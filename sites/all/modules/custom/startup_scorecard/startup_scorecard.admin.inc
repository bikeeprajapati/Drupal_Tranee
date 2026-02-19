<?php
/**
 * @file
 * Admin forms for Startup Scorecard module
 */

/**
 * Scoring form
 */
function startup_scorecard_form($form, &$form_state, $node) {
global $user;

// Check if user already scored this startup
$existing_score = db_select('startup_scorecard', 's')
->fields('s')
->condition('startup_nid', $node->nid)
->condition('rater_uid', $user->uid)
->execute()
->fetchAssoc();

$form['startup_nid'] = array(
'#type' => 'value',
'#value' => $node->nid,
);

$form['intro'] = array(
'#markup' => '<div class="scorecard-intro">
    <h2>📊 Score: ' . check_plain($node->title) . '</h2>
    <p>Rate this startup on the following criteria. Each category is worth up to 25 points.</p>
    ' . ($existing_score ? '<p class="notice">⚠️ You have already scored this startup. Submitting will update your previous score.</p>' : '') . '
</div>',
);

// Team Strength
$form['team_strength'] = array(
'#type' => 'select',
'#title' => t('👥 Team Strength'),
'#description' => t('Experience, skills, commitment, and complementary strengths of the founding team.'),
'#options' => startup_scorecard_get_score_options(25),
'#required' => TRUE,
'#default_value' => $existing_score ? $existing_score['team_strength'] : '',
);

// Market Opportunity
$form['market_opportunity'] = array(
'#type' => 'select',
'#title' => t('🎯 Market Opportunity'),
'#description' => t('Size of addressable market, growth potential, and market timing.'),
'#options' => startup_scorecard_get_score_options(25),
'#required' => TRUE,
'#default_value' => $existing_score ? $existing_score['market_opportunity'] : '',
);

// Business Model
$form['business_model'] = array(
'#type' => 'select',
'#title' => t('💰 Business Model'),
'#description' => t('Revenue streams, pricing strategy, scalability, and path to profitability.'),
'#options' => startup_scorecard_get_score_options(25),
'#required' => TRUE,
'#default_value' => $existing_score ? $existing_score['business_model'] : '',
);

// Technical Feasibility
$form['technical_feasibility'] = array(
'#type' => 'select',
'#title' => t('⚙️ Technical Feasibility'),
'#description' => t('Technology readiness, development complexity, and technical risks.'),
'#options' => startup_scorecard_get_score_options(25),
'#required' => TRUE,
'#default_value' => $existing_score ? $existing_score['technical_feasibility'] : '',
);

// Comments
$form['comments'] = array(
'#type' => 'textarea',
'#title' => t('💬 Additional Comments'),
'#description' => t('Optional feedback and recommendations for the startup.'),
'#rows' => 4,
'#default_value' => $existing_score ? $existing_score['comments'] : '',
);

$form['actions'] = array('#type' => 'actions');
$form['actions']['submit'] = array(
'#type' => 'submit',
'#value' => $existing_score ? t('Update Score') : t('Submit Score'),
'#attributes' => array('class' => array('button-primary')),
);

$form['actions']['cancel'] = array(
'#markup' => '<a href="/drupal7/?q=node/' . $node->nid . '" class="button">Cancel</a>',
);

drupal_add_css(drupal_get_path('module', 'startup_scorecard') . '/startup_scorecard.css');

return $form;
}

/**
 * Get score options for dropdown
 */
function startup_scorecard_get_score_options($max) {
$options = array('' => '- Select Score -');

for ($i = 0; $i <= $max; $i++) {
$label = $i . ' / ' . $max;

if ($i == $max) {
    $label .= ' (Outstanding)';
} 
elseif ($i >= $max * 0.8) {
    $label .= ' (Excellent)';
} 
elseif ($i >= $max * 0.6) {
    $label .= ' (Good)';
} 
elseif ($i >= $max * 0.4) {
    $label .= ' (Fair)';
} 
elseif ($i > 0) {
    $label .= ' (Poor)';
}

$options[$i] = $label;
}

return $options;
}

/**
 * Form validation
 */
function startup_scorecard_form_validate($form, &$form_state) {
// Ensure all scores are within valid range
$fields = array('team_strength', 'market_opportunity', 'business_model', 'technical_feasibility');

foreach ($fields as $field) {
$value = $form_state['values'][$field];
if ($value < 0 || $value > 25) {
    form_set_error($field, t('Score must be between 0 and 25.'));
}
}
}

/**
 * Form submission
 */
function startup_scorecard_form_submit($form, &$form_state) {
global $user;

$nid = $form_state['values']['startup_nid'];

// Calculate total score
$total = $form_state['values']['team_strength'] 
        + $form_state['values']['market_opportunity']
        + $form_state['values']['business_model']
        + $form_state['values']['technical_feasibility'];

// Check if updating existing score
$existing = db_select('startup_scorecard', 's')
->fields('s', array('score_id'))
->condition('startup_nid', $nid)
->condition('rater_uid', $user->uid)
->execute()
->fetchField();

$record = array(
'startup_nid' => $nid,
'rater_uid' => $user->uid,
'team_strength' => $form_state['values']['team_strength'],
'market_opportunity' => $form_state['values']['market_opportunity'],
'business_model' => $form_state['values']['business_model'],
'technical_feasibility' => $form_state['values']['technical_feasibility'],
'total_score' => $total,
'comments' => $form_state['values']['comments'],
'timestamp' => REQUEST_TIME,
);

if ($existing) {
// Update existing score
$record['score_id'] = $existing;
drupal_write_record('startup_scorecard', $record, 'score_id');
drupal_set_message(t('Your score has been updated successfully!'));
} 
else {
// Insert new score
drupal_write_record('startup_scorecard', $record);
drupal_set_message(t('Score submitted successfully! Total: @total/100', array('@total' => $total)));
}

// Log the scoring
watchdog('startup_scorecard', 
'User @user scored startup @nid with total @score', 
array(
    '@user' => $user->name,
    '@nid' => $nid,
    '@score' => $total,
), 
WATCHDOG_INFO
);

// Redirect back to node
$form_state['redirect'] = 'node/' . $nid;
}