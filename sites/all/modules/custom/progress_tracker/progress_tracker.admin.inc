<?php
/**
 * @file
 * Administrative pages for Progress Tracker module.
 */

function progress_tracker_milestone_templates_page() {
  $templates = progress_tracker_get_templates();
  if (empty($templates)) {
    return '<p>' . t('No milestone templates found. Click "Add Milestone Template" to create one.') . '</p>';
  }
  
  $grouped = array();
  foreach ($templates as $template) {
    $grouped[$template->category][] = $template;
  }
  
  $output = '';
  foreach ($grouped as $category => $items) {
    $output .= '<h3>' . check_plain($category) . '</h3>';
    $rows = array();
    foreach ($items as $template) {
      $rows[] = array(
        check_plain($template->title),
        check_plain(substr($template->description, 0, 100)) . '...',
        $template->default_duration_days . ' days',
        l(t('Edit'), 'admin/config/sims/milestones/' . $template->milestone_id . '/edit') . ' | ' .
        l(t('Delete'), 'admin/config/sims/milestones/' . $template->milestone_id . '/delete'),
      );
    }
    $header = array(t('Title'), t('Description'), t('Default Duration'), t('Operations'));
    $output .= theme('table', array('header' => $header, 'rows' => $rows));
  }
  return $output;
}

function progress_tracker_milestone_template_form($form, &$form_state, $milestone_id = NULL) {
  $template = NULL;
  if ($milestone_id) {
    $template = progress_tracker_load_milestone_template($milestone_id);
    if (!$template) {
      drupal_set_message(t('Milestone template not found.'), 'error');
      drupal_goto('admin/config/sims/milestones');
    }
  }
  
  $form['milestone_id'] = array('#type' => 'value', '#value' => $milestone_id);
  $form['title'] = array('#type' => 'textfield', '#title' => t('Milestone Title'), '#required' => TRUE, '#default_value' => $template ? $template->title : '', '#maxlength' => 255);
  $form['description'] = array('#type' => 'textarea', '#title' => t('Description'), '#description' => t('Detailed description of what this milestone involves.'), '#default_value' => $template ? $template->description : '', '#rows' => 5);
  $form['category'] = array('#type' => 'select', '#title' => t('Category'), '#required' => TRUE, '#options' => array('Product' => t('Product'), 'Business' => t('Business'), 'Fundraising' => t('Fundraising'), 'Team' => t('Team'), 'Marketing' => t('Marketing'), 'Operations' => t('Operations')), '#default_value' => $template ? $template->category : 'Product');
  $form['default_duration_days'] = array('#type' => 'textfield', '#title' => t('Default Duration (days)'), '#description' => t('Suggested number of days to complete this milestone.'), '#required' => TRUE, '#default_value' => $template ? $template->default_duration_days : 30, '#size' => 10);
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array('#type' => 'submit', '#value' => $milestone_id ? t('Update Template') : t('Create Template'));
  $form['actions']['cancel'] = array('#markup' => l(t('Cancel'), 'admin/config/sims/milestones'));
  return $form;
}

function progress_tracker_milestone_template_form_validate($form, &$form_state) {
  $duration = $form_state['values']['default_duration_days'];
  if (!is_numeric($duration) || $duration < 1) {
    form_set_error('default_duration_days', t('Duration must be a positive number.'));
  }
}

function progress_tracker_milestone_template_form_submit($form, &$form_state) {
  global $user;
  $record = array('title' => $form_state['values']['title'], 'description' => $form_state['values']['description'], 'category' => $form_state['values']['category'], 'default_duration_days' => $form_state['values']['default_duration_days'], 'is_template' => 1);
  if ($form_state['values']['milestone_id']) {
    $record['milestone_id'] = $form_state['values']['milestone_id'];
    drupal_write_record('progress_milestones', $record, 'milestone_id');
    drupal_set_message(t('Milestone template updated successfully.'));
  } else {
    $record['created'] = REQUEST_TIME;
    $record['created_by'] = $user->uid;
    drupal_write_record('progress_milestones', $record);
    drupal_set_message(t('Milestone template created successfully.'));
  }
  $form_state['redirect'] = 'admin/config/sims/milestones';
}

function progress_tracker_milestone_template_delete_form($form, &$form_state, $milestone_id) {
  $template = progress_tracker_load_milestone_template($milestone_id);
  if (!$template) {
    drupal_set_message(t('Milestone template not found.'), 'error');
    drupal_goto('admin/config/sims/milestones');
  }
  $form['milestone_id'] = array('#type' => 'value', '#value' => $milestone_id);
  return confirm_form($form, t('Are you sure you want to delete the milestone template %title?', array('%title' => $template->title)), 'admin/config/sims/milestones', t('This action cannot be undone.'), t('Delete'), t('Cancel'));
}

function progress_tracker_milestone_template_delete_form_submit($form, &$form_state) {
  db_delete('progress_milestones')->condition('milestone_id', $form_state['values']['milestone_id'])->execute();
  drupal_set_message(t('Milestone template deleted.'));
  $form_state['redirect'] = 'admin/config/sims/milestones';
}

function progress_tracker_admin_report() {
  drupal_add_css(drupal_get_path('module', 'progress_tracker') . '/progress_tracker.css');
  $query = db_select('progress_milestone_assignments', 'pma');
  $query->join('node', 'n', 'pma.startup_nid = n.nid');
  $query->fields('pma', array('startup_nid'));
  $query->fields('n', array('title'));
  $query->groupBy('pma.startup_nid');
  $query->groupBy('n.title');
  $startups = $query->execute()->fetchAll();
  
  if (empty($startups)) {
    return '<p>' . t('No startups have assigned milestones yet.') . '</p>';
  }
  
  $rows = array();
  foreach ($startups as $startup) {
    $stats = progress_tracker_get_startup_stats($startup->startup_nid);
    $progress_bar = '<div class="progress-bar-container"><div class="progress-bar" style="width: ' . $stats['avg_completion'] . '%"></div><span class="progress-text">' . $stats['avg_completion'] . '%</span></div>';
    $status_summary = t('@completed completed, @progress in progress, @overdue overdue', array('@completed' => $stats['completed'], '@progress' => $stats['in_progress'], '@overdue' => $stats['overdue']));
    $rows[] = array(l($startup->title, 'node/' . $startup->startup_nid . '/progress'), $stats['total'], $progress_bar, $status_summary, l(t('View Progress'), 'node/' . $startup->startup_nid . '/progress'));
  }
  
  $header = array(t('Startup'), t('Total Milestones'), t('Overall Progress'), t('Status Summary'), t('Actions'));
  $output = '<div class="progress-tracker-report"><h2>' . t('Progress Tracker - All Startups') . '</h2>' . theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('class' => array('progress-report-table')))) . '</div>';
  return $output;
}
