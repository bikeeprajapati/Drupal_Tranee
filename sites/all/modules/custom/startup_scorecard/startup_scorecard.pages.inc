<?php
/**
 * @file
 * Page callbacks for Startup Scorecard module
 */

/**
 * View all scores for a startup
 */
function startup_scorecard_view_all_scores($node) {
drupal_add_css(drupal_get_path('module', 'startup_scorecard') . '/startup_scorecard.css');

$output = '<div class="scorecard-all-scores">';
$output .= '<h2>📊 All Scores for: ' . check_plain($node->title) . '</h2>';

// Get all scores
$scores = db_select('startup_scorecard', 's')
->fields('s')
->condition('startup_nid', $node->nid)
->orderBy('timestamp', 'DESC')
->execute()
->fetchAll();

if (empty($scores)) {
$output .= '<p>No scores have been submitted for this startup yet.</p>';
$output .= '<a href="/drupal7/?q=node/' . $node->nid . '/score" class="button">⭐ Be the First to Score</a>';
} 
else {
$output .= '<table class="scores-table">';
$output .= '<thead><tr>
    <th>Rater</th>
    <th>Date</th>
    <th>Team</th>
    <th>Market</th>
    <th>Business</th>
    <th>Technical</th>
    <th>Total</th>
    <th>Grade</th>
</tr></thead><tbody>';

foreach ($scores as $score) {
    $rater = user_load($score->rater_uid);
    $badge = startup_scorecard_get_grade_badge($score->total_score);
    
    $output .= '<tr>
    <td><strong>' . check_plain($rater->name) . '</strong></td>
    <td>' . format_date($score->timestamp, 'short') . '</td>
    <td>' . $score->team_strength . '/25</td>
    <td>' . $score->market_opportunity . '/25</td>
    <td>' . $score->business_model . '/25</td>
    <td>' . $score->technical_feasibility . '/25</td>
    <td><strong>' . $score->total_score . '/100</strong></td>
    <td>' . $badge . '</td>
    </tr>';
    
    if (!empty($score->comments)) {
    $output .= '<tr class="comment-row">
        <td colspan="8">
        <div class="score-comment">
            <strong>💬 Comments:</strong> ' . check_plain($score->comments) . '
        </div>
        </td>
    </tr>';
    }
}

$output .= '</tbody></table>';

// Average score
$avg = startup_scorecard_get_average_score($node->nid);
if ($avg) {
    $output .= '<div class="average-score-box">';
    $output .= '<h3>Average Score: ' . $avg['total_score'] . '/100 ' . startup_scorecard_get_grade_badge($avg['total_score']) . '</h3>';
    $output .= '<p>Based on ' . $avg['num_ratings'] . ' rating(s)</p>';
    $output .= '</div>';
}
}

$output .= '<br/><a href="/drupal7/?q=node/' . $node->nid . '">← Back to Startup</a>';
$output .= '</div>';

return $output;
}

/**
 * Admin report page showing all startup scores
 */
function startup_scorecard_report_page() {
drupal_add_css(drupal_get_path('module', 'startup_scorecard') . '/startup_scorecard.css');

$output = '<div class="scorecard-report">';
$output .= '<h2>📊 Startup Scores Report</h2>';
$output .= '<p>Overview of all scored startups, ranked by average score.</p>';
$output .= '<hr/>';

// Get all startups with scores
$query = db_query("
SELECT 
    n.nid,
    n.title,
    AVG(s.total_score) as avg_score,
    COUNT(s.score_id) as num_ratings,
    MAX(s.timestamp) as last_scored
FROM {node} n
INNER JOIN {startup_scorecard} s ON n.nid = s.startup_nid
WHERE n.type = 'startup_idea'
GROUP BY n.nid, n.title
ORDER BY avg_score DESC
");

$results = $query->fetchAll();

if (empty($results)) {
$output .= '<p>No startups have been scored yet.</p>';
} 
else {
$output .= '<table class="scores-table report-table">';
$output .= '<thead><tr>
    <th>Rank</th>
    <th>Startup</th>
    <th>Average Score</th>
    <th>Grade</th>
    <th># Ratings</th>
    <th>Last Scored</th>
    <th>Actions</th>
</tr></thead><tbody>';

$rank = 1;
foreach ($results as $row) {
    $score = round($row->avg_score);
    $badge = startup_scorecard_get_grade_badge($score);
    
    $rank_display = $rank;
    if ($rank == 1) {
    $rank_display = '🥇';
    } 
    elseif ($rank == 2) {
    $rank_display = '🥈';
    } 
    elseif ($rank == 3) {
    $rank_display = '🥉';
    }
    
    $output .= '<tr>
    <td class="rank-cell">' . $rank_display . '</td>
    <td><a href="/drupal7/?q=node/' . $row->nid . '"><strong>' . 
        check_plain($row->title) . '</strong></a></td>
    <td><strong>' . $score . '/100</strong></td>
    <td>' . $badge . '</td>
    <td>' . $row->num_ratings . '</td>
    <td>' . format_date($row->last_scored, 'short') . '</td>
    <td>
        <a href="/drupal7/?q=node/' . $row->nid . '/scores">View Scores</a>
    </td>
    </tr>';
    
    $rank++;
}

$output .= '</tbody></table>';
}

$output .= '</div>';

return $output;
}