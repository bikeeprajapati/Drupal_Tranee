<?php

/**
 * Admin settings form.
 */
function sims_ai_free_admin_settings_form($form, &$form_state) {
$form['provider'] = array(
'#type' => 'fieldset',
'#title' => t('AI Provider'),
);

$form['provider']['sims_ai_free_provider'] = array(
'#type' => 'radios',
'#title' => t('Select AI Provider'),
'#options' => array(
    'ollama' => t('Ollama (Self-Hosted) - RECOMMENDED'),
    'huggingface' => t('Hugging Face (Free API)'),
),
'#default_value' => variable_get('sims_ai_free_provider', 'ollama'),
'#description' => t('Choose your AI backend. Ollama is recommended for unlimited usage.'),
);

// Ollama settings
$form['ollama'] = array(
'#type' => 'fieldset',
'#title' => t('Ollama Settings'),
'#states' => array(
    'visible' => array(
    ':input[name="sims_ai_free_provider"]' => array('value' => 'ollama'),
    ),
),
);

$form['ollama']['sims_ai_free_ollama_url'] = array(
'#type' => 'textfield',
'#title' => t('Ollama URL'),
'#default_value' => variable_get('sims_ai_free_ollama_url', 'http://localhost:11434'),
'#description' => t('URL of your Ollama server (default: http://localhost:11434)'),
);

$form['ollama']['sims_ai_free_ollama_model'] = array(
'#type' => 'select',
'#title' => t('Model'),
'#options' => array(
    'phi' => t('Phi (Recommended - Fast)'),
    'llama2' => t('Llama 2 (Better quality)'),
    'mistral' => t('Mistral (Best for analysis)'),
    'llama2:13b' => t('Llama 2 13B (High quality, slow)'),
),
'#default_value' => variable_get('sims_ai_free_ollama_model', 'phi'),
'#description' => t('Choose the AI model to use.'),
);

// Hugging Face settings
$form['huggingface'] = array(
'#type' => 'fieldset',
'#title' => t('Hugging Face Settings'),
'#states' => array(
    'visible' => array(
    ':input[name="sims_ai_free_provider"]' => array('value' => 'huggingface'),
    ),
),
);

$form['huggingface']['sims_ai_free_hf_token'] = array(
'#type' => 'textfield',
'#title' => t('API Token'),
'#default_value' => variable_get('sims_ai_free_hf_token', ''),
'#description' => t('Get your free token at <a href="@url" target="_blank">huggingface.co/settings/tokens</a>', 
    array('@url' => 'https://huggingface.co/settings/tokens')),
);

$form['huggingface']['sims_ai_free_hf_model'] = array(
'#type' => 'select',
'#title' => t('Model'),
'#options' => array(
    'mistralai/Mistral-7B-Instruct-v0.2' => t('Mistral 7B (Recommended)'),
    'meta-llama/Llama-2-7b-chat-hf' => t('Llama 2 Chat'),
    'microsoft/phi-2' => t('Phi-2'),
),
'#default_value' => variable_get('sims_ai_free_hf_model', 'mistralai/Mistral-7B-Instruct-v0.2'),
);

// Cache settings
$form['cache'] = array(
'#type' => 'fieldset',
'#title' => t('Cache Settings'),
);

$form['cache']['sims_ai_free_cache_enabled'] = array(
'#type' => 'checkbox',
'#title' => t('Enable response caching'),
'#default_value' => variable_get('sims_ai_free_cache_enabled', TRUE),
'#description' => t('Cache AI responses to improve speed.'),
);

$form['cache']['sims_ai_free_cache_ttl'] = array(
'#type' => 'select',
'#title' => t('Cache lifetime'),
'#options' => array(
    3600 => t('1 hour'),
    86400 => t('24 hours'),
    604800 => t('7 days'),
),
'#default_value' => variable_get('sims_ai_free_cache_ttl', 86400),
);

return system_settings_form($form);
}