<?php
/**
 * @file
 * User-facing pages for Progress Tracker module.
 */

function progress_tracker_startup_view($node) {
  drupal_add_css(drupal_get_path('module', 'progress_tracker') . '/progress_tracker.css');
  drupal_add_js(drupal_get_path('module', 'progress_tracker') . '/progress_tracker.js');
  $milestones = progress_tracker_load_startup_milestones($node->nid);
  $stats = progress_tracker_get_startup_stats($node->nid);
  $output = '<div class="progress-tracker-page">';
  $output .= theme('progress_tracker_stats_widget', array('stats' => $stats));
  if (!empty($milestones)) {
    $output .= '<h2>' . t('Milestone Timeline') . '</h2>';
    $output .= theme('progress_tracker_timeline', array('milestones' => $milestones, 'startup_nid' => $node->nid));
  } else {
    $output .= '<p class="no-milestones">' . t('No milestones assigned yet. Click "Assign Milestone" to get started.') . '</p>';
  }
  $output .= '</div>';
  return $output;
}

function progress_tracker_assign_milestone_form($form, &$form_state, $node) {
  $templates = progress_tracker_get_templates();
  if (empty($templates)) {
    drupal_set_message(t('No milestone templates available.'), 'warning');
    drupal_goto('node/' . $node->nid . '/progress');
  }
  $options = array();
  foreach ($templates as $template) {
    $options[$template->category][$template->milestone_id] = $template->title . ' (' . $template->default_duration_days . ' days)';
  }
  $form['startup_nid'] = array('#type' => 'value', '#value' => $node->nid);
  $form['startup_title'] = array('#type' => 'value', '#value' => $node->title);
  $form['milestone_id'] = array('#type' => 'select', '#title' => t('Select Milestone'), '#required' => TRUE, '#options' => $options);
  $form['target_date'] = array('#type' => 'date_popup', '#title' => t('Target Completion Date'), '#required' => TRUE, '#default_value' => date('Y-m-d', strtotime('+30 days')), '#date_format' => 'Y-m-d');
  $form['priority'] = array('#type' => 'select', '#title' => t('Priority'), '#required' => TRUE, '#options' => array('low' => t('Low'), 'medium' => t('Medium'), 'high' => t('High'), 'critical' => t('Critical')), '#default_value' => 'medium');
  $form['notes'] = array('#type' => 'textarea', '#title' => t('Initial Notes'), '#rows' => 3);
  $form['send_email'] = array('#type' => 'checkbox', '#title' => t('Send email notification to founder'), '#default_value' => 1);
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Assign Milestone'));
  $form['actions']['cancel'] = array('#markup' => l(t('Cancel'), 'node/' . $node->nid . '/progress'));
  return $form;
}

function progress_tracker_assign_milestone_form_submit($form, &$form_state) {
  global $user;
  $template = progress_tracker_load_milestone_template($form_state['values']['milestone_id']);
  $record = array('startup_nid' => $form_state['values']['startup_nid'], 'milestone_id' => $form_state['values']['milestone_id'], 'milestone_title' => $template->title, 'target_date' => strtotime($form_state['values']['target_date']), 'status' => 'not_started', 'completion_percentage' => 0, 'priority' => $form_state['values']['priority'], 'notes' => $form_state['values']['notes'], 'assigned_by' => $user->uid, 'assigned_date' => REQUEST_TIME, 'last_updated' => REQUEST_TIME, 'updated_by' => $user->uid);
  drupal_write_record('progress_milestone_assignments', $record);
  if ($form_state['values']['send_email']) {
    $startup = node_load($form_state['values']['startup_nid']);
    $founder = user_load($startup->uid);
    $params = array('milestone_title' => $template->title, 'startup_title' => $form_state['values']['startup_title'], 'target_date' => format_date(strtotime($form_state['values']['target_date']), 'custom', 'F j, Y'), 'priority' => ucfirst($form_state['values']['priority']), 'url' => url('node/' . $startup->nid . '/progress', array('absolute' => TRUE)));
    drupal_mail('progress_tracker', 'milestone_assigned', $founder->mail, language_default(), $params);
  }
  drupal_set_message(t('Milestone "@milestone" assigned successfully!', array('@milestone' => $template->title)));
  $form_state['redirect'] = 'node/' . $form_state['values']['startup_nid'] . '/progress';
}

function progress_tracker_update_form($form, &$form_state, $assignment_id) {
  $assignment = progress_tracker_load_assignment($assignment_id);
  if (!$assignment) {
    drupal_set_message(t('Milestone assignment not found.'), 'error');
    drupal_goto('');
  }
  $form['assignment_id'] = array('#type' => 'value', '#value' => $assignment_id);
  $form['old_status'] = array('#type' => 'value', '#value' => $assignment->status);
  $form['old_percentage'] = array('#type' => 'value', '#value' => $assignment->completion_percentage);
  $form['milestone_info'] = array('#markup' => '<div class="milestone-update-header"><h2>' . t('Update Progress: @title', array('@title' => $assignment->milestone_title)) . '</h2></div>');
  $form['status'] = array('#type' => 'select', '#title' => t('Status'), '#required' => TRUE, '#options' => array('not_started' => t('Not Started'), 'in_progress' => t('In Progress'), 'completed' => t('Completed'), 'delayed' => t('Delayed')), '#default_value' => $assignment->status);
  $form['completion_percentage'] = array('#type' => 'select', '#title' => t('Completion Percentage'), '#required' => TRUE, '#options' => drupal_map_assoc(range(0, 100, 10)), '#default_value' => $assignment->completion_percentage);
  $form['update_note'] = array('#type' => 'textarea', '#title' => t('Progress Update Notes'), '#rows' => 4);
  $form['actual_date'] = array('#type' => 'date_popup', '#title' => t('Actual Completion Date'), '#date_format' => 'Y-m-d', '#states' => array('visible' => array(':input[name="status"]' => array('value' => 'completed'))));
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Update Progress'));
  $form['actions']['cancel'] = array('#markup' => l(t('Cancel'), 'node/' . $assignment->startup_nid . '/progress'));
  return $form;
}

function progress_tracker_update_form_submit($form, &$form_state) {
  global $user;
  $assignment_id = $form_state['values']['assignment_id'];
  $assignment = progress_tracker_load_assignment($assignment_id);
  $record = array('assignment_id' => $assignment_id, 'status' => $form_state['values']['status'], 'completion_percentage' => $form_state['values']['completion_percentage'], 'last_updated' => REQUEST_TIME, 'updated_by' => $user->uid);
  if (!empty($form_state['values']['update_note'])) {
    $existing_notes = $assignment->notes ? $assignment->notes . "\n\n---\n\n" : '';
    $record['notes'] = $existing_notes . format_date(REQUEST_TIME, 'custom', 'Y-m-d H:i') . ' - ' . $user->name . ":\n" . $form_state['values']['update_note'];
  }
  if ($form_state['values']['status'] == 'completed' && !empty($form_state['values']['actual_date'])) {
    $record['actual_date'] = strtotime($form_state['values']['actual_date']);
  }
  drupal_write_record('progress_milestone_assignments', $record, 'assignment_id');
  $history = array('assignment_id' => $assignment_id, 'old_status' => $form_state['values']['old_status'], 'new_status' => $form_state['values']['status'], 'old_percentage' => $form_state['values']['old_percentage'], 'new_percentage' => $form_state['values']['completion_percentage'], 'update_note' => $form_state['values']['update_note'], 'updated_by' => $user->uid, 'update_timestamp' => REQUEST_TIME);
  drupal_write_record('progress_updates', $history);
  drupal_set_message(t('Progress updated successfully!'));
  $form_state['redirect'] = 'node/' . $assignment->startup_nid . '/progress';
}

function progress_tracker_delete_assignment_form($form, &$form_state, $assignment_id) {
  $assignment = progress_tracker_load_assignment($assignment_id);
  if (!$assignment) {
    drupal_set_message(t('Milestone assignment not found.'), 'error');
    drupal_goto('');
  }
  $form['assignment_id'] = array('#type' => 'value', '#value' => $assignment_id);
  $form['startup_nid'] = array('#type' => 'value', '#value' => $assignment->startup_nid);
  return confirm_form($form, t('Are you sure you want to remove this milestone?'), 'node/' . $assignment->startup_nid . '/progress', t('Milestone: %title', array('%title' => $assignment->milestone_title)), t('Remove'), t('Cancel'));
}

function progress_tracker_delete_assignment_form_submit($form, &$form_state) {
  db_delete('progress_milestone_assignments')->condition('assignment_id', $form_state['values']['assignment_id'])->execute();
  drupal_set_message(t('Milestone removed.'));
  $form_state['redirect'] = 'node/' . $form_state['values']['startup_nid'] . '/progress';
}

function theme_progress_tracker_timeline($variables) {
  $milestones = $variables['milestones'];
  $output = '<div class="progress-timeline">';
  foreach ($milestones as $milestone) {
    $status_class = 'status-' . str_replace('_', '-', $milestone->status);
    $priority_class = 'priority-' . $milestone->priority;
    $is_overdue = ($milestone->target_date && $milestone->target_date < REQUEST_TIME && $milestone->status != 'completed');
    if ($is_overdue) $status_class .= ' overdue';
    $output .= '<div class="milestone-item ' . $status_class . ' ' . $priority_class . '">';
    $output .= '<div class="milestone-status-icon">';
    if ($milestone->status == 'completed') {
      $output .= '<span class="icon-complete">✓</span>';
    } elseif ($is_overdue) {
      $output .= '<span class="icon-overdue">!</span>';
    } else {
      $output .= '<span class="icon-progress">' . $milestone->completion_percentage . '%</span>';
    }
    $output .= '</div><div class="milestone-content">';
    $output .= '<h3 class="milestone-title">' . check_plain($milestone->milestone_title) . '</h3>';
    $output .= '<div class="milestone-progress-bar"><div class="progress-fill" style="width: ' . $milestone->completion_percentage . '%"></div></div>';
    $output .= '<div class="milestone-meta">';
    $output .= '<span class="milestone-status">' . ucwords(str_replace('_', ' ', $milestone->status)) . '</span> | ';
    $output .= '<span class="milestone-priority">Priority: ' . ucfirst($milestone->priority) . '</span> | ';
    $output .= '<span class="milestone-date">Target: ' . ($milestone->target_date ? format_date($milestone->target_date, 'custom', 'M j, Y') : 'Not set') . '</span>';
    if ($milestone->status == 'completed' && $milestone->actual_date) {
      $output .= ' | <span class="milestone-completed">Completed: ' . format_date($milestone->actual_date, 'custom', 'M j, Y') . '</span>';
    }
    $output .= '</div><div class="milestone-actions">';
    $output .= l(t('Update Progress'), 'progress/milestone/' . $milestone->assignment_id . '/update', array('attributes' => array('class' => array('button', 'button-update')))) . ' ';
    $output .= l(t('Remove'), 'progress/milestone/' . $milestone->assignment_id . '/delete', array('attributes' => array('class' => array('button', 'button-delete'))));
    $output .= '</div></div></div>';
  }
  $output .= '</div>';
  return $output;
}

function theme_progress_tracker_stats_widget($variables) {
  $stats = $variables['stats'];
  if ($stats['total'] == 0) return '';
  $output = '<div class="progress-stats-widget"><h2>' . t('Progress Overview') . '</h2><div class="stats-grid">';
  $output .= '<div class="stat-box stat-overall"><div class="stat-label">' . t('Overall Progress') . '</div><div class="stat-value">' . $stats['avg_completion'] . '%</div><div class="stat-bar"><div class="stat-fill" style="width: ' . $stats['avg_completion'] . '%"></div></div></div>';
  $output .= '<div class="stat-box stat-total"><div class="stat-label">' . t('Total Milestones') . '</div><div class="stat-value">' . $stats['total'] . '</div></div>';
  $output .= '<div class="stat-box stat-completed"><div class="stat-label">' . t('Completed') . '</div><div class="stat-value">' . $stats['completed'] . '</div></div>';
  $output .= '<div class="stat-box stat-in-progress"><div class="stat-label">' . t('In Progress') . '</div><div class="stat-value">' . $stats['in_progress'] . '</div></div>';
  if ($stats['overdue'] > 0) {
    $output .= '<div class="stat-box stat-overdue"><div class="stat-label">' . t('Overdue') . '</div><div class="stat-value">' . $stats['overdue'] . '</div></div>';
  }
  $output .= '</div></div>';
  return $output;
}
