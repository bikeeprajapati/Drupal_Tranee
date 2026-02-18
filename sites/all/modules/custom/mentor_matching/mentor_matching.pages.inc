<?php
/**
 * @file
 * Page callbacks for Mentor Matching module
 */

/**
 * Page callback for mentor matching results
 */
function mentor_matching_results_page($node) {
module_load_include('inc', 'mentor_matching', 'mentor_matching.scoring');

if ($node->type != 'startup_idea') {
drupal_not_found();
return;
}

$industry = mentor_matching_get_startup_industry($node);
$mentors = mentor_matching_get_all_mentors();
$scored_mentors = mentor_matching_score_mentors($mentors, $industry);
$output = mentor_matching_build_results_page($node, $industry, $scored_mentors);

drupal_add_css(drupal_get_path('module', 'mentor_matching') . '/mentor_matching.css');

return $output;
}

/**
 * Get startup's industry from node
 */
function mentor_matching_get_startup_industry($node) {
if (!empty($node->field_industry[LANGUAGE_NONE][0]['tid'])) {
$term = taxonomy_term_load($node->field_industry[LANGUAGE_NONE][0]['tid']);
return $term->name;
}
return '';
}

/**
 * Get all mentors from database
 */
function mentor_matching_get_all_mentors() {
$mentor_role = user_role_load_by_name('Mentor');

if (!$mentor_role) {
return array();
}

$query = db_select('users', 'u')
->fields('u', array('uid', 'name', 'mail'))
->condition('u.status', 1);

$query->join('users_roles', 'ur', 'u.uid = ur.uid');
$query->condition('ur.rid', $mentor_role->rid);

return $query->execute()->fetchAll();
}

/**
 * Build the results page HTML
 */
function mentor_matching_build_results_page($node, $industry, $scored_mentors) {
$output = '<div class="mentor-matching-results">';
$output .= '<h2>🎯 Mentor Matching Results</h2>';
$output .= '<h3>Startup: <em>' . check_plain($node->title) . '</em></h3>';
$output .= '<h4>Industry: <strong>' . check_plain($industry) . '</strong></h4>';
$output .= '<hr/>';

if (empty($scored_mentors)) {
$output .= '<p>No mentors found in the system.</p>';
} 
else {
$output .= '<p>Found <strong>' . count($scored_mentors) . '</strong> available mentor(s):</p>';
$output .= mentor_matching_build_results_table($node, $scored_mentors);
}

$output .= '<br/><a href="/drupal7/?q=node/' . $node->nid . '">← Back to Startup</a>';
$output .= '</div>';

return $output;
}

/**
 * Build the results table with auto-assign buttons
 */
function mentor_matching_build_results_table($node, $scored_mentors) {
$output = '<table class="mentor-match-table">';
$output .= '<thead><tr>
<th>Mentor Name</th>
<th>Match Score</th>
<th>Actions</th>
</tr></thead><tbody>';

$is_first = TRUE;

foreach ($scored_mentors as $item) {
$mentor = $item['mentor'];
$score = $item['score'];

$badge = mentor_matching_get_score_badge($score);

$auto_assign_url = '/drupal7/?q=mentor-matching/' . $node->nid . '/auto-assign/' . $mentor->uid;

$actions = '<a href="' . $auto_assign_url . '" class="button auto-assign-btn" 
                onclick="return confirm(\'Assign ' . check_plain($mentor->name) . ' to this startup?\')">
                ⚡ Quick Assign
            </a>';

if ($is_first && $score >= 80) {
    $actions = '<span class="best-match-badge">🏆 BEST MATCH</span><br/>' . $actions;
    $is_first = FALSE;
}

$output .= '<tr>
    <td><strong>' . check_plain($mentor->name) . '</strong></td>
    <td>' . $badge . ' (' . $score . '/100)</td>
    <td>' . $actions . '</td>
</tr>';
}

$output .= '</tbody></table>';

return $output;
}

/**
 * Get score badge HTML
 */
function mentor_matching_get_score_badge($score) {
if ($score >= 80) {
return '<span class="match-excellent">⭐ Excellent Match</span>';
} 
elseif ($score >= 50) {
return '<span class="match-good">✅ Good Match</span>';
} 
else {
return '<span class="match-low">🔵 Available</span>';
}
}

/**
 * Auto-assign the best matching mentor to a startup
 */
function mentor_matching_auto_assign($node, $mentor_uid) {
global $user;

if ($node->type != 'startup_idea') {
drupal_set_message(t('Invalid startup node.'), 'error');
drupal_goto('node/' . $node->nid);
return;
}

$mentor = user_load($mentor_uid);
if (!$mentor) {
drupal_set_message(t('Invalid mentor.'), 'error');
drupal_goto('mentor-matching/' . $node->nid);
return;
}

// Calculate match score for history
module_load_include('inc', 'mentor_matching', 'mentor_matching.scoring');
$industry = mentor_matching_get_startup_industry($node);

// Create a mentor object with required fields for scoring
$mentor_obj = new stdClass();
$mentor_obj->uid = $mentor->uid;
$mentor_obj->name = $mentor->name;
$mentor_obj->mail = $mentor->mail;

$score = mentor_matching_calculate_score($mentor_obj, $industry);

// Log to history BEFORE assignment
mentor_matching_log_assignment(
$node->nid,
$node->title,
$mentor->uid,
$mentor->name,
$score,
$industry,
$user->uid,
'auto'
);

// Assign the mentor
$node->field_assigned_mentor[LANGUAGE_NONE][0]['uid'] = $mentor_uid;
node_save($node);

// Send email notification
mentor_matching_send_assignment_email($mentor, $node);

drupal_set_message(
t('Successfully assigned @mentor to startup "@title"! An email notification has been sent.', 
    array(
    '@mentor' => $mentor->name,
    '@title' => $node->title,
    )
), 
'status'
);

watchdog('mentor_matching', 
'Auto-assigned mentor @mentor (UID: @uid) to startup @title (NID: @nid)', 
array(
    '@mentor' => $mentor->name,
    '@uid' => $mentor_uid,
    '@title' => $node->title,
    '@nid' => $node->nid,
), 
WATCHDOG_INFO
);

drupal_goto('node/' . $node->nid);
}

/**
 * Send email notification to mentor when assigned
 */
function mentor_matching_send_assignment_email($mentor, $node) {
global $base_url;

$industry = '';
if (!empty($node->field_industry[LANGUAGE_NONE][0]['tid'])) {
$term = taxonomy_term_load($node->field_industry[LANGUAGE_NONE][0]['tid']);
$industry = $term->name;
}

$founder = user_load($node->uid);

$params = array(
'subject' => t('New Startup Assignment: @title', array('@title' => $node->title)),
'body' => t(
    "Dear @mentor,

You have been assigned as a mentor for a new startup through our Smart Mentor Matching system!

🚀 Startup Details:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
- Name: @title
- Industry: @industry
- Founder: @founder
- Submitted: @date

📊 Why You Were Matched:
You were selected as the best match based on your expertise in @industry and current availability.

🎯 Next Steps:
1. Review the startup details at: @url
2. Contact the founder to schedule an initial meeting
3. Begin the mentoring relationship

Log in to view full startup details:
@base_url/drupal7/?q=mentor/startups

Best regards,
InnovateX Incubator Team

---
This is an automated email from the Mentor Matching System.",
    array(
    '@mentor' => $mentor->name,
    '@title' => $node->title,
    '@industry' => $industry,
    '@founder' => $founder->name,
    '@date' => format_date($node->created, 'medium'),
    '@url' => $base_url . '/drupal7/?q=node/' . $node->nid,
    '@base_url' => $base_url,
    )
),
);

$result = drupal_mail('mentor_matching', 'mentor_assigned', $mentor->mail, language_default(), $params);

if ($result['result']) {
watchdog('mentor_matching', 
    'Assignment email sent to @mentor (@email) for startup @title', 
    array(
    '@mentor' => $mentor->name,
    '@email' => $mentor->mail,
    '@title' => $node->title,
    ), 
    WATCHDOG_INFO
);
}

return $result['result'];
}

/**
 * Log mentor assignment to history table
 */
function mentor_matching_log_assignment($startup_nid, $startup_title, $mentor_uid, $mentor_name, $score, $industry, $assigned_by_uid, $method = 'auto') {
$record = array(
'startup_nid' => $startup_nid,
'startup_title' => $startup_title,
'mentor_uid' => $mentor_uid,
'mentor_name' => $mentor_name,
'match_score' => $score,
'industry' => $industry,
'assigned_by_uid' => $assigned_by_uid,
'assignment_method' => $method,
'timestamp' => REQUEST_TIME,
);

drupal_write_record('mentor_matching_history', $record);

watchdog('mentor_matching', 
'Logged assignment: @mentor to @startup (Score: @score, Method: @method)', 
array(
    '@mentor' => $mentor_name,
    '@startup' => $startup_title,
    '@score' => $score,
    '@method' => $method,
), 
WATCHDOG_INFO
);
}

/**
 * Page callback for match history
 */
function mentor_matching_history_page() {
drupal_add_css(drupal_get_path('module', 'mentor_matching') . '/mentor_matching.css');

$output = '<div class="mentor-matching-results">';
$output .= '<h2>📊 Mentor Matching History</h2>';
$output .= '<p>Complete audit trail of all mentor assignments made through the Smart Matching system.</p>';
$output .= '<hr/>';

// Get all history records
$query = db_select('mentor_matching_history', 'h')
->fields('h')
->orderBy('timestamp', 'DESC')
->extend('PagerDefault')
->limit(20);

$results = $query->execute()->fetchAll();

if (empty($results)) {
$output .= '<p>No assignment history found yet.</p>';
} 
else {
$output .= '<table class="mentor-match-table history-table">';
$output .= '<thead><tr>
    <th>Date/Time</th>
    <th>Startup</th>
    <th>Mentor</th>
    <th>Industry</th>
    <th>Score</th>
    <th>Method</th>
    <th>Assigned By</th>
</tr></thead><tbody>';

foreach ($results as $record) {
    $assigned_by = user_load($record->assigned_by_uid);
    
    $method_badge = $record->assignment_method == 'auto' 
    ? '<span class="method-auto">⚡ Auto</span>' 
    : '<span class="method-manual">👤 Manual</span>';
    
    $score_badge = mentor_matching_get_score_badge($record->match_score);
    
    $output .= '<tr>
    <td>' . format_date($record->timestamp, 'short') . '</td>
    <td><a href="/drupal7/?q=node/' . $record->startup_nid . '">' . 
        check_plain($record->startup_title) . '</a></td>
    <td><strong>' . check_plain($record->mentor_name) . '</strong></td>
    <td>' . check_plain($record->industry) . '</td>
    <td>' . $score_badge . ' (' . $record->match_score . ')</td>
    <td>' . $method_badge . '</td>
    <td>' . check_plain($assigned_by->name) . '</td>
    </tr>';
}

$output .= '</tbody></table>';
$output .= theme('pager');
}

$output .= '</div>';

return $output;
}