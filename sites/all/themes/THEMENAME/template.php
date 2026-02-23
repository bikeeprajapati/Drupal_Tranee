<?php

/**
 * @file
 * Bootstrap sub-theme.
 *
 * Place your custom PHP code in this file.
 */
/**
 * Redirect anonymous users from Startup Dashboard to login page.
 */
function mytheme_preprocess_page(&$variables) {
  // Check if the user is anonymous
if (user_is_anonymous()) {
    // Get the current path
    $current_path = current_path();
    
    // Redirect if this is the Startup Dashboard page
    if ($current_path == 'incubator/startups') {
    drupal_goto('user/login');
    }
}

}
/**
 * Implements hook_preprocess_page().
 */
function THEMENAME_preprocess_page(&$variables) {
  // Add dashboard CSS
  drupal_add_css(drupal_get_path('theme', 'THEMENAME') . '/css/dashboard.css', array(
    'group' => CSS_THEME,
    'weight' => 100,
  ));
}
