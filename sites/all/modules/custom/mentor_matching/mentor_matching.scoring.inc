<?php
/**
 * @file
 * Scoring algorithms for Mentor Matching module
 */

/**
 * Score all mentors and return sorted array
 */
function mentor_matching_score_mentors($mentors, $industry) {
$scored_mentors = array();

foreach ($mentors as $mentor) {
    $score = mentor_matching_calculate_score($mentor, $industry);
    
    // Only include mentors with score > 0
    if ($score > 0) {
    $scored_mentors[] = array(
    'mentor' => $mentor,
    'score' => $score,
    );
    }
}

// Sort by score (highest first)
usort($scored_mentors, function($a, $b) {
return $b['score'] - $a['score'];
});

return $scored_mentors;
}

/**
 * Calculate match score for a single mentor
 * 
 * @param object $mentor
 *   Mentor user object with uid, name, mail
 * @param string $industry
 *   Startup's industry name
 * 
 * @return int
 *   Score from 0-100
 */
function mentor_matching_calculate_score($mentor, $industry) {
  $score = 50; // Base score

// Load full mentor profile
$mentor_user = user_load($mentor->uid);

// Factor 1: Availability Status (most important)
$score = mentor_matching_apply_availability_factor($mentor_user, $score);

// If unavailable, stop here
if ($score == 0) {
return 0;
}

// Factor 2: Industry Match
$score = mentor_matching_apply_industry_factor($mentor_user, $industry, $score);

// Factor 3: Current Workload
$score = mentor_matching_apply_workload_factor($mentor_user, $score);

// Ensure score is between 0-100
return max(0, min(100, $score));
}

/**
 * Apply availability status to score
 */
function mentor_matching_apply_availability_factor($mentor_user, $score) {
if (!empty($mentor_user->field_mentor_availability[LANGUAGE_NONE][0]['value'])) {
$availability = $mentor_user->field_mentor_availability[LANGUAGE_NONE][0]['value'];

switch ($availability) {
    case 'unavailable':
    return 0; // Don't show unavailable mentors
    
    case 'busy':
    $score -= 30; // Significant penalty for busy mentors
    break;
    
    case 'available':
    $score += 10; // Bonus for explicitly available
    break;
}
}

return $score;
}

/**
 * Apply industry match to score
 */
function mentor_matching_apply_industry_factor($mentor_user, $industry, $score) {
if (!empty($mentor_user->field_industry[LANGUAGE_NONE])) {
foreach ($mentor_user->field_industry[LANGUAGE_NONE] as $mentor_industry) {
    $term = taxonomy_term_load($mentor_industry['tid']);
    
    if ($term && strtolower($term->name) == strtolower($industry)) {
    return 95; // Perfect match! Override base score
    }
}
}

return $score;
}

/**
 * Apply current workload to score
 */
function mentor_matching_apply_workload_factor($mentor_user, $score) {
$current_assignments = db_select('field_data_field_assigned_mentor', 'f')
    ->condition('f.field_assigned_mentor_uid', $mentor_user->uid)
    ->countQuery()
    ->execute()
    ->fetchField();

  // Penalty based on workload
if ($current_assignments >= 3) {
$score -= 20;
} 
elseif ($current_assignments >= 1) {
$score -= 10;
}

return $score;
}