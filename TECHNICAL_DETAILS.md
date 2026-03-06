# 🔧 SIMS Technical Documentation
## Detailed Technical Specifications

**Project:** Startup Incubator Management System  
**Developer:** Bikee Prajapati  
**Last Updated:** March 2026

---

## 📋 Table of Contents

1. [System Architecture](#system-architecture)
2. [Database Design](#database-design)
3. [Module Specifications](#module-specifications)
4. [API Documentation](#api-documentation)
5. [Security Implementation](#security-implementation)
6. [Testing Strategy](#testing-strategy)
7. [Performance Optimization](#performance-optimization)

---

## 1. System Architecture

### High-Level Architecture
```
┌─────────────────────────────────────────┐
│         Presentation Layer              │
│  ┌──────────────┐  ┌─────────────────┐ │
│  │  Bootstrap   │  │  Custom CSS/JS  │ │
│  │  Theme 3.x   │  │  jQuery         │ │
│  └──────────────┘  └─────────────────┘ │
└─────────────────────────────────────────┘
                    ↕
┌─────────────────────────────────────────┐
│       Application Logic Layer           │
│  ┌────────┐ ┌────────┐ ┌─────────────┐ │
│  │ Views  │ │ Rules  │ │   Custom    │ │
│  │  (5)   │ │  (8)   │ │  Modules(4) │ │
│  └────────┘ └────────┘ └─────────────┘ │
└─────────────────────────────────────────┘
                    ↕
┌─────────────────────────────────────────┐
│          Data Access Layer              │
│  ┌─────────────────────────────────┐   │
│  │   Drupal Database API           │   │
│  │   Entity API                    │   │
│  └─────────────────────────────────┘   │
└─────────────────────────────────────────┘
                    ↕
┌─────────────────────────────────────────┐
│           Database Layer                │
│  ┌────────┐ ┌──────────┐ ┌──────────┐  │
│  │  Node  │ │  Custom  │ │  Field   │  │
│  │ Tables │ │  Tables  │ │  Tables  │  │
│  └────────┘ └──────────┘ └──────────┘  │
└─────────────────────────────────────────┘
```

### Technology Stack

| Layer | Technology | Version | Purpose |
|-------|-----------|---------|---------|
| **CMS** | Drupal | 7.98 | Content management framework |
| **Backend** | PHP | 7.4 | Business logic |
| **Database** | MySQL | 5.7 | Data persistence |
| **Web Server** | Apache | 2.4 | HTTP server |
| **Frontend Framework** | Bootstrap | 3.x | Responsive UI |
| **JavaScript** | jQuery | 1.x | Interactive features |
| **CLI Tool** | Drush | 8.x | Admin tasks |

---

## 2. Database Design

### Entity Relationship Diagram
```
User (Founder)
    │
    │ 1:N
    ↓
Startup Idea ←───── 1:1 ─────→ User (Mentor)
    │                               │
    │ 1:N                          │
    ↓                               ↓
Scorecard                    Mentor Matching
    │                               │
    ↓                               ↓
Progress Assignments ←─── N:1 ──→ Progress Templates
    │
    │ 1:N
    ↓
Progress Updates
```

### Database Tables

#### Custom Tables (6)

**1. mentor_matching_history**
```sql
CREATE TABLE mentor_matching_history (
  match_id INT AUTO_INCREMENT PRIMARY KEY,
  startup_nid INT NOT NULL,
  mentor_uid INT NOT NULL,
  assigned_date INT NOT NULL,
  assigned_by INT NOT NULL,
  notes TEXT,
  status VARCHAR(20) DEFAULT 'active',
  INDEX(startup_nid),
  INDEX(mentor_uid),
  FOREIGN KEY (startup_nid) REFERENCES node(nid) ON DELETE CASCADE
);
```

**2. startup_scorecard**
```sql
CREATE TABLE startup_scorecard (
  score_id INT AUTO_INCREMENT PRIMARY KEY,
  startup_nid INT NOT NULL,
  mentor_uid INT NOT NULL,
  team_score INT CHECK (team_score BETWEEN 1 AND 10),
  market_score INT CHECK (market_score BETWEEN 1 AND 10),
  product_score INT CHECK (product_score BETWEEN 1 AND 10),
  business_model_score INT CHECK (business_model_score BETWEEN 1 AND 10),
  traction_score INT CHECK (traction_score BETWEEN 1 AND 10),
  total_score DECIMAL(5,2),
  comments TEXT,
  scored_date INT NOT NULL,
  INDEX(startup_nid),
  INDEX(mentor_uid)
);
```

**3. progress_milestones**
```sql
CREATE TABLE progress_milestones (
  milestone_id INT AUTO_INCREMENT PRIMARY KEY,
  title VARCHAR(255) NOT NULL,
  description TEXT,
  category ENUM('Product','Business','Fundraising','Team','Marketing'),
  is_template TINYINT(1) DEFAULT 1,
  default_duration_days INT,
  created INT NOT NULL,
  created_by INT NOT NULL,
  INDEX(category),
  INDEX(is_template)
);
```

**4. progress_milestone_assignments**
```sql
CREATE TABLE progress_milestone_assignments (
  assignment_id INT AUTO_INCREMENT PRIMARY KEY,
  startup_nid INT NOT NULL,
  milestone_id INT NOT NULL,
  milestone_title VARCHAR(255),
  target_date INT,
  actual_date INT,
  status ENUM('not_started','in_progress','completed','delayed','overdue') 
    DEFAULT 'not_started',
  completion_percentage INT DEFAULT 0 CHECK (completion_percentage BETWEEN 0 AND 100),
  priority ENUM('low','medium','high','critical') DEFAULT 'medium',
  notes TEXT,
  assigned_by INT NOT NULL,
  assigned_date INT NOT NULL,
  last_updated INT,
  updated_by INT,
  INDEX(startup_nid),
  INDEX(status),
  INDEX(target_date),
  FOREIGN KEY (startup_nid) REFERENCES node(nid) ON DELETE CASCADE
);
```

**5. progress_updates**
```sql
CREATE TABLE progress_updates (
  update_id INT AUTO_INCREMENT PRIMARY KEY,
  assignment_id INT NOT NULL,
  old_status VARCHAR(20),
  new_status VARCHAR(20),
  old_percentage INT,
  new_percentage INT,
  update_note TEXT,
  updated_by INT NOT NULL,
  update_timestamp INT NOT NULL,
  INDEX(assignment_id),
  FOREIGN KEY (assignment_id) REFERENCES progress_milestone_assignments(assignment_id)
    ON DELETE CASCADE
);
```

**6. feedback**
```sql
CREATE TABLE feedback (
  feedback_id INT AUTO_INCREMENT PRIMARY KEY,
  feedback_type ENUM('mentor','milestone','resource','general') NOT NULL,
  related_id INT,
  rating INT CHECK (rating BETWEEN 1 AND 5),
  comment TEXT,
  submitted_by INT NOT NULL,
  created INT NOT NULL,
  status ENUM('new','reviewed','archived') DEFAULT 'new',
  INDEX(feedback_type),
  INDEX(submitted_by),
  INDEX(status)
);
```

### Indexing Strategy

**Primary Indexes:**
- All primary keys (automatic)

**Foreign Key Indexes:**
- `startup_nid` - Fast lookups for startup-related data
- `mentor_uid` - Quick mentor queries
- `assigned_id` - Update history lookups

**Query Optimization Indexes:**
- `status` - Filter by status
- `target_date` - Date-based queries
- `category` - Group by category
- `feedback_type` - Filter feedback

---

## 3. Module Specifications

### 3.1 Progress Tracker Module

**File Structure:**
```
progress_tracker/
├── progress_tracker.info (391 bytes)
├── progress_tracker.module (11 KB)
├── progress_tracker.install (8.8 KB)
├── progress_tracker.admin.inc (7.8 KB)
├── progress_tracker.pages.inc (16 KB)
├── progress_tracker.css (6.8 KB)
├── progress_tracker.js (2.1 KB)
└── README.md (3.7 KB)
```

**Key Functions:**
```php
/**
 * Get startup statistics.
 *
 * @param int $startup_nid
 *   Node ID of startup.
 *
 * @return array
 *   Statistics array with keys:
 *   - total: Total milestone count
 *   - completed: Completed count
 *   - in_progress: In progress count
 *   - not_started: Not started count
 *   - overdue: Overdue count
 *   - avg_completion: Average percentage
 */
function progress_tracker_get_startup_stats($startup_nid);

/**
 * Load milestones for a startup.
 *
 * @param int $startup_nid
 *   Node ID of startup.
 *
 * @return array
 *   Array of milestone objects.
 */
function progress_tracker_load_startup_milestones($startup_nid);

/**
 * Save milestone assignment.
 *
 * @param array $assignment
 *   Assignment data array.
 *
 * @return int|bool
 *   Assignment ID on success, FALSE on failure.
 */
function progress_tracker_save_assignment($assignment);
```

**Hooks Implemented:**
- `hook_menu()` - 9 menu items
- `hook_permission()` - 5 permissions
- `hook_theme()` - 2 theme functions
- `hook_mail()` - Email integration
- `hook_node_view()` - Tab integration

---

### 3.2 Feedback Entity Module

**Entity Definition:**
```php
function feedback_entity_entity_info() {
  $info['feedback'] = array(
    'label' => t('Feedback'),
    'entity class' => 'Entity',
    'controller class' => 'EntityAPIController',
    'base table' => 'feedback',
    'fieldable' => FALSE,
    'entity keys' => array(
      'id' => 'feedback_id',
    ),
    'bundles' => array(
      'feedback' => array(
        'label' => 'Feedback',
        'admin' => array(
          'path' => 'admin/content/feedback',
          'access arguments' => array('administer feedback'),
        ),
      ),
    ),
    'view modes' => array(
      'full' => array(
        'label' => t('Full'),
        'custom settings' => FALSE,
      ),
    ),
    'uri callback' => 'entity_class_uri',
    'module' => 'feedback_entity',
    'access callback' => 'feedback_entity_access',
  );
  return $info;
}
```

**CRUD Operations:**
```php
// CREATE
$feedback = entity_create('feedback', array(
  'feedback_type' => 'mentor',
  'rating' => 5,
  'comment' => 'Great session!',
  'submitted_by' => $user->uid,
  'created' => REQUEST_TIME,
  'status' => 'new',
));
entity_save('feedback', $feedback);

// READ
$feedback = entity_load_single('feedback', $feedback_id);
$feedbacks = entity_load('feedback', array($id1, $id2));

// UPDATE
$feedback->status = 'reviewed';
entity_save('feedback', $feedback);

// DELETE
entity_delete('feedback', $feedback_id);
```

---

## 4. API Documentation

### REST-like Endpoints (Menu Callbacks)

| Path | Method | Access | Purpose |
|------|--------|--------|---------|
| `/node/%node/progress` | GET | Authenticated | View progress timeline |
| `/node/%node/progress/assign` | GET/POST | Manager | Assign milestone form |
| `/progress/milestone/%/update` | GET/POST | Founder | Update progress form |
| `/progress/milestone/%/delete` | GET/POST | Manager | Delete milestone |
| `/feedback/submit` | GET/POST | Authenticated | Submit feedback |
| `/admin/content/feedback` | GET | Admin | View all feedback |
| `/admin/reports/progress-tracker` | GET | Manager | Progress report |

### Function API

**Statistics API:**
```php
$stats = progress_tracker_get_startup_stats(123);
// Returns:
// array(
//   'total' => 10,
//   'completed' => 3,
//   'in_progress' => 5,
//   'not_started' => 2,
//   'overdue' => 1,
//   'avg_completion' => 45,
// )
```

**Mentor Matching API:**
```php
$match_id = mentor_matching_assign_mentor(123, 456);
$startups = mentor_matching_get_mentor_startups(456);
```

---

## 5. Security Implementation

### Input Validation

**Form Validation:**
```php
function progress_tracker_update_form_validate($form, &$form_state) {
  $percentage = $form_state['values']['completion_percentage'];
  
  // Type checking
  if (!is_numeric($percentage)) {
    form_set_error('completion_percentage', t('Must be a number.'));
  }
  
  // Range checking
  if ($percentage < 0 || $percentage > 100) {
    form_set_error('completion_percentage', t('Must be between 0 and 100.'));
  }
}
```

**SQL Injection Prevention:**
```php
// WRONG - Don't do this
$result = db_query("SELECT * FROM feedback WHERE uid = " . $uid);

// CORRECT - Use placeholders
$result = db_query("SELECT * FROM {feedback} WHERE submitted_by = :uid", 
  array(':uid' => $uid));
```

**XSS Prevention:**
```php
// Always sanitize output
$output = check_plain($user_input);           // For plain text
$output = filter_xss($user_input);            // For HTML
$output = check_markup($text, $format);       // For filtered HTML
```

### Access Control

**Permission Checks:**
```php
function progress_tracker_startup_access($node) {
  global $user;
  
  // Type check
  if ($node->type != 'startup_idea') {
    return FALSE;
  }
  
  // Admin access
  if (user_access('administer progress tracker')) {
    return TRUE;
  }
  
  // Owner access
  if ($node->uid == $user->uid && user_access('update own progress')) {
    return TRUE;
  }
  
  // Manager/Mentor access
  if (user_access('update all progress')) {
    return TRUE;
  }
  
  return FALSE;
}
```

---

## 6. Testing Strategy

### Test Coverage

**Unit Tests:**
- Function-level testing
- Input validation
- Edge cases

**Integration Tests:**
- Database operations
- Form submissions
- Email notifications

**Security Tests:**
- Permission validation
- Access control
- Input sanitization

### Test Implementation
```php
class FeedbackEntitySimpleTest extends DrupalWebTestCase {
  
  public function setUp() {
    parent::setUp('entity', 'feedback_entity');
    $this->founder = $this->drupalCreateUser(array('submit feedback'));
    $this->admin = $this->drupalCreateUser(array('administer feedback'));
  }
  
  public function testFounderCanSubmitFeedback() {
    $this->drupalLogin($this->founder);
    $this->drupalGet('feedback/submit');
    $this->assertResponse(200);
    
    $edit = array(
      'feedback_type' => 'mentor',
      'rating' => 5,
      'comment' => 'Test feedback',
    );
    $this->drupalPost('feedback/submit', $edit, t('Submit Feedback'));
    $this->assertText('Thank you for your feedback!');
    
    $feedback = db_query("SELECT * FROM {feedback} WHERE submitted_by = :uid", 
      array(':uid' => $this->founder->uid))->fetchObject();
    $this->assertTrue($feedback !== FALSE);
  }
}
```

---

## 7. Performance Optimization

### Database Optimization

**Indexes:**
- Added indexes on frequently queried columns
- Composite indexes for common WHERE clauses

**Query Optimization:**
```php
// Efficient - Single query
$query = db_select('progress_milestone_assignments', 'pma')
  ->fields('pma')
  ->condition('startup_nid', $startup_nid)
  ->condition('status', 'overdue')
  ->orderBy('target_date', 'ASC')
  ->range(0, 10);

$milestones = $query->execute()->fetchAll();
```

### Caching Strategy

**Menu Caching:**
```php
$items['node/%node/progress'] = array(
  'title' => 'Progress Tracker',
  'page callback' => 'progress_tracker_startup_view',
  'page arguments' => array(1),
  'access callback' => 'progress_tracker_startup_access',
  'access arguments' => array(1),
  'type' => MENU_LOCAL_TASK,
  'weight' => 5,
);
```

### CSS/JS Aggregation

**Drupal Settings:**
- CSS aggregation: Enabled
- JS aggregation: Enabled
- CSS/JS compression: Enabled

---

## 8. Code Quality Standards

### Coding Standards

**Followed:**
- Drupal Coding Standards
- PHP DocBlock comments
- Consistent naming conventions
- Proper indentation (2 spaces)

**Example:**
```php
/**
 * Calculate average score from scorecard criteria.
 *
 * @param array $scores
 *   Array of individual scores.
 *
 * @return float
 *   Average score rounded to 2 decimal places.
 */
function startup_scorecard_calculate_average($scores) {
  $total = array_sum($scores);
  $count = count($scores);
  
  if ($count == 0) {
    return 0.00;
  }
  
  return round($total / $count, 2);
}
```

---

## 📞 Technical Support

**Developer:** Bikee Prajapati  
**Email:** your.email@example.com  
**GitHub:** https://github.com/bikeeprajapati  

---

*This technical documentation is maintained alongside the codebase.*  
*Last Updated: March 2026*
