<?php

/**
 * Add 403 theme suggestion
 */
function THEMENAME_preprocess_page(&$vars) {
  if (drupal_get_http_header('Status') == '403 Forbidden') {
    $vars['theme_hook_suggestions'][] = 'page__403';
  }
}

/**
 * Status badges + currency formatting
 */
function THEMENAME_preprocess_views_view_field(&$vars) {
  $field = $vars['field'];

  if ($field->field == 'field_startup_status') {
    $value = $vars['output'];
    $badges = array(
      'Funded'      => array('bg' => '#d1fae5', 'color' => '#065f46'),
      'Selected'    => array('bg' => '#dbeafe', 'color' => '#1e40af'),
      'Screening'   => array('bg' => '#fef3c7', 'color' => '#d97706'),
      'Shortlisted' => array('bg' => '#ede9fe', 'color' => '#6d28d9'),
      'Rejected'    => array('bg' => '#fee2e2', 'color' => '#dc2626'),
    );
    foreach ($badges as $status => $style) {
      if (strpos($value, $status) !== FALSE) {
        $vars['output'] = '<span style="background:' . $style['bg'] . ';color:' . $style['color'] . ';padding:5px 14px;border-radius:20px;font-size:12px;font-weight:700;text-transform:uppercase;letter-spacing:0.05em;">' . $status . '</span>';
        break;
      }
    }
  }

  if ($field->field == 'field_funding_requested') {
    $raw = trim(strip_tags((string)$vars['output']));
    if (is_numeric($raw) && $raw > 0) {
      $vars['output'] = '$' . number_format((float)$raw, 0, '.', ',');
    }
  }
}
