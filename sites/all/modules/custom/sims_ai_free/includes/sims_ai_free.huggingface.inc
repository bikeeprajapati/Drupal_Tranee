<?php

/**
 * @file
 * Hugging Face API integration
 */

/**
 * Generate text using Hugging Face.
 */
function sims_ai_free_huggingface_generate($prompt, $history = array()) {
$api_token = variable_get('sims_ai_free_hf_token', '');
$model = variable_get('sims_ai_free_hf_model', 'mistralai/Mistral-7B-Instruct-v0.2');

if (empty($api_token)) {
watchdog('sims_ai_free', 'Hugging Face token not configured', array(), WATCHDOG_ERROR);
return FALSE;
}

// Build full prompt with history
$full_prompt = "You are an expert startup advisor.\n\n";

foreach ($history as $msg) {
$role = $msg->role === 'user' ? 'User' : 'Assistant';
$full_prompt .= "$role: " . $msg->content . "\n";
}

$full_prompt .= "User: $prompt\nAssistant:";

// Prepare request
$data = array(
'inputs' => $full_prompt,
'parameters' => array(
    'max_new_tokens' => 500,
    'temperature' => 0.7,
    'top_p' => 0.95,
    'return_full_text' => FALSE,
),
);

$ch = curl_init("https://api-inference.huggingface.co/models/$model");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
curl_setopt($ch, CURLOPT_POST, TRUE);
curl_setopt($ch, CURLOPT_HTTPHEADER, array(
'Content-Type: application/json',
'Authorization: Bearer ' . $api_token,
));
curl_setopt($ch, CURLOPT_POSTFIELDS, json_encode($data));
curl_setopt($ch, CURLOPT_TIMEOUT, 30);

$response = curl_exec($ch);
$http_code = curl_getinfo($ch, CURLINFO_HTTP_CODE);
curl_close($ch);

if ($http_code !== 200) {
return FALSE;
}

$result = json_decode($response, TRUE);

if (!$result) {
return FALSE;
}

// Handle different response formats
$text = '';
if (isset($result[0]['generated_text'])) {
$text = $result[0]['generated_text'];
}
elseif (isset($result['generated_text'])) {
$text = $result['generated_text'];
}
else {
return FALSE;
}

return array(
'text' => trim($text),
'model' => $model,
);
}