<?php

/**
 * @file
 * Ollama API integration
 */

/**
 * Send chat request to Ollama.
 */
function sims_ai_free_ollama_chat($prompt, $history = array()) {
$ollama_url = variable_get('sims_ai_free_ollama_url', 'http://localhost:11434');
$model = variable_get('sims_ai_free_ollama_model', 'phi');

// Build messages array
$messages = array();

// Add system prompt
$messages[] = array(
'role' => 'system',
'content' => 'You are an expert startup advisor helping founders refine their pitch. Provide specific, actionable feedback. Be encouraging but honest. Keep responses concise and practical.',
);

// Add conversation history
foreach ($history as $msg) {
$messages[] = array(
    'role' => $msg->role,
    'content' => $msg->content,
);
}

// Add current prompt
$messages[] = array(
'role' => 'user',
'content' => $prompt,
);

// Prepare request
$data = array(
'model' => $model,
'messages' => $messages,
'stream' => FALSE,
);

// Make request
$start_time = microtime(TRUE);

$ch = curl_init($ollama_url . '/api/chat');
curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
curl_setopt($ch, CURLOPT_POST, TRUE);
curl_setopt($ch, CURLOPT_HTTPHEADER, array('Content-Type: application/json'));
curl_setopt($ch, CURLOPT_POSTFIELDS, json_encode($data));
curl_setopt($ch, CURLOPT_TIMEOUT, 60);

$response = curl_exec($ch);
$http_code = curl_getinfo($ch, CURLINFO_HTTP_CODE);
$error = curl_error($ch);
curl_close($ch);

$response_time = round((microtime(TRUE) - $start_time) * 1000);

// Handle errors
if ($error) {
watchdog('sims_ai_free', 'Ollama cURL error: @error', array('@error' => $error), WATCHDOG_ERROR);
return FALSE;
}

if ($http_code !== 200) {
watchdog('sims_ai_free', 'Ollama HTTP error: @code', array('@code' => $http_code), WATCHDOG_ERROR);
return FALSE;
}

// Parse response
$result = json_decode($response, TRUE);

if (!$result || !isset($result['message']['content'])) {
watchdog('sims_ai_free', 'Invalid Ollama response', array(), WATCHDOG_ERROR);
return FALSE;
}

return array(
'text' => $result['message']['content'],
'response_time' => $response_time,
'model' => $model,
);
}

/**
 * Simple generate (without chat history).
 */
function sims_ai_free_ollama_generate($prompt) {
$ollama_url = variable_get('sims_ai_free_ollama_url', 'http://localhost:11434');
$model = variable_get('sims_ai_free_ollama_model', 'phi');

$data = array(
'model' => $model,
'prompt' => $prompt,
'stream' => FALSE,
);

$ch = curl_init($ollama_url . '/api/generate');
curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
curl_setopt($ch, CURLOPT_POST, TRUE);
curl_setopt($ch, CURLOPT_HTTPHEADER, array('Content-Type: application/json'));
curl_setopt($ch, CURLOPT_POSTFIELDS, json_encode($data));
curl_setopt($ch, CURLOPT_TIMEOUT, 60);

$response = curl_exec($ch);
$http_code = curl_getinfo($ch, CURLINFO_HTTP_CODE);
curl_close($ch);

if ($http_code !== 200) {
return FALSE;
}

$result = json_decode($response, TRUE);

if (!$result || !isset($result['response'])) {
return FALSE;
}

return array(
'text' => $result['response'],
'model' => $model,
);
}

/**
 * Test Ollama connection.
 */
function sims_ai_free_ollama_test_connection() {
$ollama_url = variable_get('sims_ai_free_ollama_url', 'http://localhost:11434');

$ch = curl_init($ollama_url . '/api/tags');
curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
curl_setopt($ch, CURLOPT_TIMEOUT, 5);

$response = curl_exec($ch);
$http_code = curl_getinfo($ch, CURLINFO_HTTP_CODE);
curl_close($ch);

if ($http_code === 200) {
$result = json_decode($response, TRUE);
return isset($result['models']) ? $result['models'] : TRUE;
}

return FALSE;
}