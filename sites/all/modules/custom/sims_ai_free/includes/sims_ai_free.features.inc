<?php

/**
 * @file
 * AI features for different user roles.
 */

/**
 * Founder: Application quality scorer.
 * 
 * Analyzes a startup submission and provides quality score.
 */
function sims_ai_free_score_application($node) {
if ($node->type !== 'startup_idea') {
return FALSE;
}

// Check cache first
$insight = sims_ai_free_get_insight($node->nid, 'quality_score');
if ($insight) {
return unserialize($insight->data);
}

// Build analysis prompt
$prompt = "Analyze this startup application and score it out of 100:

Startup Name: {$node->title}
Problem Statement: " . (isset($node->field_problem_statement[LANGUAGE_NONE][0]['value']) ? $node->field_problem_statement[LANGUAGE_NONE][0]['value'] : 'Not provided') . "
Market Opportunity: " . (isset($node->field_market_opportunity[LANGUAGE_NONE][0]['value']) ? $node->field_market_opportunity[LANGUAGE_NONE][0]['value'] : 'Not provided') . "
Revenue Model: " . (isset($node->field_revenue_model[LANGUAGE_NONE][0]['value']) ? $node->field_revenue_model[LANGUAGE_NONE][0]['value'] : 'Not provided') . "

Provide:
1. Overall Score (0-100)
2. Problem Clarity Score (0-25)
3. Market Opportunity Score (0-25)
4. Revenue Model Score (0-25)
5. Overall Pitch Quality Score (0-25)
6. Top 3 Strengths (brief)
7. Top 3 Areas for Improvement (brief)

Format your response as:
OVERALL_SCORE: [number]
PROBLEM_SCORE: [number]
MARKET_SCORE: [number]
REVENUE_SCORE: [number]
PITCH_SCORE: [number]
STRENGTHS:
- [strength 1]
- [strength 2]
- [strength 3]
IMPROVEMENTS:
- [improvement 1]
- [improvement 2]
- [improvement 3]";

// Call AI
$provider = variable_get('sims_ai_free_provider', 'ollama');
if ($provider === 'ollama') {
module_load_include('inc', 'sims_ai_free', 'includes/sims_ai_free.ollama');
$response = sims_ai_free_ollama_generate($prompt);
}
else {
module_load_include('inc', 'sims_ai_free', 'includes/sims_ai_free.huggingface');
$response = sims_ai_free_huggingface_generate($prompt, array());
}

if (!$response) {
return FALSE;
}

// Parse response
$text = $response['text'];
$scores = sims_ai_free_parse_scores($text);

// Save insight
sims_ai_free_save_insight('node', $node->nid, 'quality_score', $scores, $scores['overall']);

return $scores;
}

/**
 * Parse AI response for scores.
 */
function sims_ai_free_parse_scores($text) {
$scores = array(
'overall' => 0,
'problem' => 0,
'market' => 0,
'revenue' => 0,
'pitch' => 0,
'strengths' => array(),
'improvements' => array(),
'raw_response' => $text,
);

// Extract scores using regex
if (preg_match('/OVERALL_SCORE:\s*(\d+)/', $text, $matches)) {
$scores['overall'] = (int) $matches[1];
}
if (preg_match('/PROBLEM_SCORE:\s*(\d+)/', $text, $matches)) {
$scores['problem'] = (int) $matches[1];
}
if (preg_match('/MARKET_SCORE:\s*(\d+)/', $text, $matches)) {
$scores['market'] = (int) $matches[1];
}
if (preg_match('/REVENUE_SCORE:\s*(\d+)/', $text, $matches)) {
$scores['revenue'] = (int) $matches[1];
}
if (preg_match('/PITCH_SCORE:\s*(\d+)/', $text, $matches)) {
$scores['pitch'] = (int) $matches[1];
}

// Extract strengths
if (preg_match('/STRENGTHS:(.*?)(?=IMPROVEMENTS:|$)/s', $text, $matches)) {
$strengths_text = $matches[1];
preg_match_all('/[-•]\s*(.+)$/m', $strengths_text, $strength_matches);
$scores['strengths'] = array_slice($strength_matches[1], 0, 3);
}

// Extract improvements
if (preg_match('/IMPROVEMENTS:(.*?)$/s', $text, $matches)) {
$improvements_text = $matches[1];
preg_match_all('/[-•]\s*(.+)$/m', $improvements_text, $improvement_matches);
$scores['improvements'] = array_slice($improvement_matches[1], 0, 3);
}

return $scores;
}

/**
 * Admin: Screen startup application.
 */
function sims_ai_free_screen_application($node) {
if ($node->type !== 'startup_idea') {
return FALSE;
}

// Check cache
$insight = sims_ai_free_get_insight($node->nid, 'screening_summary');
if ($insight) {
return unserialize($insight->data);
}

// Build screening prompt
$prompt = "You are screening startup applications for an incubator program. Analyze this application:

Startup: {$node->title}
Founder: " . (isset($node->uid) ? user_load($node->uid)->name : 'Unknown') . "
Stage: " . (isset($node->field_stage[LANGUAGE_NONE][0]['value']) ? $node->field_stage[LANGUAGE_NONE][0]['value'] : 'Not specified') . "
Industry: " . (isset($node->field_industry[LANGUAGE_NONE][0]['value']) ? $node->field_industry[LANGUAGE_NONE][0]['value'] : 'Not specified') . "

Problem Statement:
" . (isset($node->field_problem_statement[LANGUAGE_NONE][0]['value']) ? $node->field_problem_statement[LANGUAGE_NONE][0]['value'] : 'Not provided') . "

Market Opportunity:
" . (isset($node->field_market_opportunity[LANGUAGE_NONE][0]['value']) ? $node->field_market_opportunity[LANGUAGE_NONE][0]['value'] : 'Not provided') . "

Revenue Model:
" . (isset($node->field_revenue_model[LANGUAGE_NONE][0]['value']) ? $node->field_revenue_model[LANGUAGE_NONE][0]['value'] : 'Not provided') . "

Provide:
1. Executive Summary (2-3 sentences)
2. Key Strengths (3 bullet points)
3. Major Concerns (3 bullet points)
4. Recommendation: ACCEPT, REVIEW, or REJECT
5. Score: 0-100
6. Reason for recommendation (1 sentence)

Be concise and actionable.";

// Call AI
$provider = variable_get('sims_ai_free_provider', 'ollama');
if ($provider === 'ollama') {
module_load_include('inc', 'sims_ai_free', 'includes/sims_ai_free.ollama');
$response = sims_ai_free_ollama_generate($prompt);
}
else {
module_load_include('inc', 'sims_ai_free', 'includes/sims_ai_free.huggingface');
$response = sims_ai_free_huggingface_generate($prompt, array());
}

if (!$response) {
return FALSE;
}

$screening = array(
'summary' => $response['text'],
'node_id' => $node->nid,
'screened_at' => REQUEST_TIME,
'provider' => $provider,
);

// Save insight
sims_ai_free_save_insight('node', $node->nid, 'screening_summary', $screening);

return $screening;
}

/**
 * Mentor: Match mentor to startups.
 */
function sims_ai_free_match_mentor($mentor_uid, $startup_nids) {
$mentor = user_load($mentor_uid);

if (!$mentor) {
return FALSE;
}

// Get mentor profile info
$mentor_profile = array(
'name' => $mentor->name,
'industries' => array(), // You can add field_industries if you have it
'expertise' => array(),  // You can add field_expertise if you have it
);

$matches = array();

foreach ($startup_nids as $nid) {
$node = node_load($nid);
if (!$node || $node->type !== 'startup_idea') {
    continue;
}

// Build matching prompt
$prompt = "Match this mentor with this startup and provide a match score:

Mentor: {$mentor->name}
Mentor Expertise: General startup mentoring

Startup: {$node->title}
Industry: " . (isset($node->field_industry[LANGUAGE_NONE][0]['value']) ? $node->field_industry[LANGUAGE_NONE][0]['value'] : 'Not specified') . "
Stage: " . (isset($node->field_stage[LANGUAGE_NONE][0]['value']) ? $node->field_stage[LANGUAGE_NONE][0]['value'] : 'Not specified') . "

Provide:
1. Match Score (0-100)
2. Why this is a good match (2-3 sentences)
3. Key areas where mentor can help (3 bullet points)

Format:
MATCH_SCORE: [number]
REASONING: [your reasoning]
HELP_AREAS:
- [area 1]
- [area 2]
- [area 3]";

// Call AI
$provider = variable_get('sims_ai_free_provider', 'ollama');
if ($provider === 'ollama') {
    module_load_include('inc', 'sims_ai_free', 'includes/sims_ai_free.ollama');
    $response = sims_ai_free_ollama_generate($prompt);
}
else {
    module_load_include('inc', 'sims_ai_free', 'includes/sims_ai_free.huggingface');
    $response = sims_ai_free_huggingface_generate($prompt, array());
}

if ($response) {
    // Parse match score
    $score = 50; // Default
    if (preg_match('/MATCH_SCORE:\s*(\d+)/', $response['text'], $matches_regex)) {
    $score = (int) $matches_regex[1];
    }
    
    $matches[] = array(
    'nid' => $nid,
    'title' => $node->title,
    'score' => $score,
    'analysis' => $response['text'],
    );
}
}

// Sort by score
usort($matches, function($a, $b) {
return $b['score'] - $a['score'];
});

return $matches;
}

/**
 * Investor: Generate startup summary.
 */
function sims_ai_free_investor_summary($node) {
if ($node->type !== 'startup_idea') {
return FALSE;
}

// Check cache
$insight = sims_ai_free_get_insight($node->nid, 'investor_summary');
if ($insight) {
return unserialize($insight->data);
}

// Build investor summary prompt
$prompt = "Create an investor-focused summary for this startup:

Startup: {$node->title}
Industry: " . (isset($node->field_industry[LANGUAGE_NONE][0]['value']) ? $node->field_industry[LANGUAGE_NONE][0]['value'] : 'Not specified') . "
Stage: " . (isset($node->field_stage[LANGUAGE_NONE][0]['value']) ? $node->field_stage[LANGUAGE_NONE][0]['value'] : 'Not specified') . "

Problem: " . (isset($node->field_problem_statement[LANGUAGE_NONE][0]['value']) ? $node->field_problem_statement[LANGUAGE_NONE][0]['value'] : 'Not provided') . "

Market: " . (isset($node->field_market_opportunity[LANGUAGE_NONE][0]['value']) ? $node->field_market_opportunity[LANGUAGE_NONE][0]['value'] : 'Not provided') . "

Revenue Model: " . (isset($node->field_revenue_model[LANGUAGE_NONE][0]['value']) ? $node->field_revenue_model[LANGUAGE_NONE][0]['value'] : 'Not provided') . "

Provide:
1. Executive Summary (3-4 sentences)
2. Market Opportunity (1-2 sentences)
3. Competitive Advantage (1-2 sentences)
4. Investment Highlights (3 bullet points)
5. Key Risks (3 bullet points)
6. Investment Readiness Score (0-100)

Keep it concise and investor-focused.";

// Call AI
$provider = variable_get('sims_ai_free_provider', 'ollama');
if ($provider === 'ollama') {
module_load_include('inc', 'sims_ai_free', 'includes/sims_ai_free.ollama');
$response = sims_ai_free_ollama_generate($prompt);
}
else {
module_load_include('inc', 'sims_ai_free', 'includes/sims_ai_free.huggingface');
$response = sims_ai_free_huggingface_generate($prompt, array());
}

if (!$response) {
return FALSE;
}

$summary = array(
'content' => $response['text'],
'generated_at' => REQUEST_TIME,
'node_id' => $node->nid,
);

// Save insight
sims_ai_free_save_insight('node', $node->nid, 'investor_summary', $summary);

return $summary;
}

/**
 * Helper: Get insight.
 */
function sims_ai_free_get_insight($entity_id, $insight_type, $entity_type = 'node') {
return db_select('sims_ai_insights', 'i')
->fields('i')
->condition('entity_type', $entity_type)
->condition('entity_id', $entity_id)
->condition('insight_type', $insight_type)
->orderBy('updated', 'DESC')
->range(0, 1)
->execute()
->fetchObject();
}

/**
 * Helper: Save insight.
 */
function sims_ai_free_save_insight($entity_type, $entity_id, $insight_type, $data, $score = NULL) {
$existing = sims_ai_free_get_insight($entity_id, $insight_type, $entity_type);

if ($existing) {
db_update('sims_ai_insights')
    ->fields(array(
    'data' => serialize($data),
    'score' => $score,
    'updated' => REQUEST_TIME,
    ))
    ->condition('insight_id', $existing->insight_id)
    ->execute();
}
else {
db_insert('sims_ai_insights')
    ->fields(array(
    'entity_type' => $entity_type,
    'entity_id' => $entity_id,
    'insight_type' => $insight_type,
    'data' => serialize($data),
    'score' => $score,
    'created' => REQUEST_TIME,
    'updated' => REQUEST_TIME,
    ))
    ->execute();
}
}