<?php

/**
 * @file
 * Cache management functions for SIMS AI Free.
 */

/**
 * Generate cache key from prompt and options.
 */
function sims_ai_free_generate_cache_key($prompt, $feature = 'general') {
$key_data = array(
'prompt' => $prompt,
'feature' => $feature,
'provider' => variable_get('sims_ai_free_provider', 'ollama'),
);

return 'sims_ai_free:' . md5(serialize($key_data));
}

/**
 * Get cached response.
 */
function sims_ai_free_get_cached_response($cache_key) {
// Check if caching is enabled
if (!variable_get('sims_ai_free_cache_enabled', TRUE)) {
return FALSE;
}

// Try Drupal cache first
$cache = cache_get($cache_key, 'cache');
if ($cache && $cache->expire > REQUEST_TIME) {
// Update hit count
db_update('sims_ai_cache')
    ->expression('hit_count', 'hit_count + 1')
    ->condition('cache_key', $cache_key)
    ->execute();
    
return $cache->data;
}

// Try database cache
$cached = db_select('sims_ai_cache', 'c')
->fields('c')
->condition('cache_key', $cache_key)
->condition('expires', REQUEST_TIME, '>')
->execute()
->fetchObject();

if ($cached) {
// Update hit count
db_update('sims_ai_cache')
    ->expression('hit_count', 'hit_count + 1')
    ->condition('cache_id', $cached->cache_id)
    ->execute();
    
// Also set in Drupal cache for faster retrieval
cache_set($cache_key, $cached->response_data, 'cache', $cached->expires);

return $cached->response_data;
}

return FALSE;
}

/**
 * Cache AI response.
 */
function sims_ai_free_cache_response($cache_key, $response, $feature = 'general') {
// Check if caching is enabled
if (!variable_get('sims_ai_free_cache_enabled', TRUE)) {
return;
}

$ttl = variable_get('sims_ai_free_cache_ttl', 86400);
$expires = REQUEST_TIME + $ttl;

// Store in Drupal cache
cache_set($cache_key, $response, 'cache', $expires);

// Store in database for statistics
db_merge('sims_ai_cache')
->key(array('cache_key' => $cache_key))
->fields(array(
    'feature' => $feature,
    'response_data' => $response,
    'created' => REQUEST_TIME,
    'expires' => $expires,
    'hit_count' => 0,
))
->execute();
}

/**
 * Clear expired cache entries.
 */
function sims_ai_free_clear_expired_cache() {
$deleted = db_delete('sims_ai_cache')
->condition('expires', REQUEST_TIME, '<')
->execute();

if ($deleted > 0) {
watchdog('sims_ai_free', 'Cleared @count expired cache entries', 
    array('@count' => $deleted), WATCHDOG_INFO);
}

return $deleted;
}

/**
 * Clear all AI cache.
 */
function sims_ai_free_clear_all_cache() {
// Clear database cache
db_truncate('sims_ai_cache')->execute();

// Clear Drupal cache
cache_clear_all('sims_ai_free:', 'cache', TRUE);

drupal_set_message(t('All AI cache cleared.'));
watchdog('sims_ai_free', 'All AI cache cleared manually', array(), WATCHDOG_INFO);
}

/**
 * Get cache statistics.
 */
function sims_ai_free_get_cache_stats() {
$stats = array();

// Total cached responses
$stats['total'] = db_select('sims_ai_cache', 'c')
->countQuery()
->execute()
->fetchField();

// Valid (not expired) cache entries
$stats['valid'] = db_select('sims_ai_cache', 'c')
->condition('expires', REQUEST_TIME, '>')
->countQuery()
->execute()
->fetchField();

// Expired entries
$stats['expired'] = $stats['total'] - $stats['valid'];

// Total hits
$stats['total_hits'] = db_query('SELECT SUM(hit_count) FROM {sims_ai_cache}')
->fetchField();

// Most hit cache entries
$stats['top_cached'] = db_select('sims_ai_cache', 'c')
->fields('c', array('feature', 'hit_count', 'created'))
->orderBy('hit_count', 'DESC')
->range(0, 5)
->execute()
->fetchAll();

// Cache by feature
$stats['by_feature'] = db_select('sims_ai_cache', 'c')
->fields('c', array('feature'))
->addExpression('COUNT(*)', 'count')
->addExpression('SUM(hit_count)', 'hits')
->groupBy('feature')
->execute()
->fetchAll();

return $stats;
}

/**
 * Warm up cache with common queries.
 */
function sims_ai_free_warmup_cache() {
$common_prompts = array(
array(
    'prompt' => 'What makes a good startup pitch?',
    'feature' => 'chatbot',
),
array(
    'prompt' => 'What are the key elements of a startup business plan?',
    'feature' => 'chatbot',
),
array(
    'prompt' => 'How do I validate my startup idea?',
    'feature' => 'chatbot',
),
);

$provider = variable_get('sims_ai_free_provider', 'ollama');
$warmed = 0;

foreach ($common_prompts as $item) {
$cache_key = sims_ai_free_generate_cache_key($item['prompt'], $item['feature']);

// Skip if already cached
if (sims_ai_free_get_cached_response($cache_key)) {
    continue;
}

// Generate and cache response
if ($provider === 'ollama') {
    module_load_include('inc', 'sims_ai_free', 'includes/sims_ai_free.ollama');
    $response = sims_ai_free_ollama_generate($item['prompt']);
}
else {
    module_load_include('inc', 'sims_ai_free', 'includes/sims_ai_free.huggingface');
    $response = sims_ai_free_huggingface_generate($item['prompt'], array());
}

if ($response) {
    sims_ai_free_cache_response($cache_key, $response['text'], $item['feature']);
    $warmed++;
}
}

drupal_set_message(t('Warmed up @count cache entries.', array('@count' => $warmed)));
return $warmed;
}