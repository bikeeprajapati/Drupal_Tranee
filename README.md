# SIMS - Startup Incubator Management System

**Developed by:** Bikee Prajapati  
**Training Program:** Virtual Training Session - Associate Software Engineer (PHP/Drupal)  
**Duration:** 2-3 weeks  
**Platform:** Drupal 7.98

---

## Project Overview

SIMS (Startup Incubator Management System) is a comprehensive web-based platform designed to digitize and automate the entire startup incubation lifecycle. The system addresses the critical challenges faced by startup incubators who currently manage operations using fragmented manual processes (Excel spreadsheets, emails, WhatsApp).

### Problem Statement

Startup incubators face several operational challenges:
- Manual tracking of startup applications and progress
- Inefficient mentor-startup matching processes
- Lack of structured progress monitoring systems
- Poor visibility for investors and stakeholders
- No centralized data repository or reporting capabilities
- Time-consuming administrative tasks

### Solution Delivered

SIMS provides an integrated platform that:
- **Automates workflow processes** - From submission to funding
- **Centralizes all data** - Single source of truth for operations
- **Enables real-time visibility** - Role-based dashboards for all stakeholders
- **Tracks progress systematically** - Visual milestone-based tracking
- **Facilitates communication** - Automated email notifications
- **Supports decision-making** - Comprehensive reporting and analytics

### Key Features

**Multi-Role System:**
- 5 distinct user roles (Admin, Incubator Manager, Mentor, Founder, Investor)
- Role-based permissions and access control
- Customized dashboards for each role

**Core Functionality:**
- Startup submission and screening workflow
- Mentor-startup assignment system
- Visual progress tracking with milestones
- Multi-criteria scorecard evaluation
- Feedback collection and moderation
- Automated email notifications
- Comprehensive reporting

**Technical Highlights:**
- 4 custom modules (~3,500 lines of code)
- 6 normalized database tables
- 5 role-specific dashboards
- 8 automated workflow rules
- 56 test assertions (100% pass rate)
- Production-ready code quality

---

## Technology Stack

| Component | Technology | Version |
|-----------|-----------|---------|
| **CMS Framework** | Drupal | 7.98 |
| **Backend Language** | PHP | 7.4 |
| **Database** | MySQL | 5.7 |
| **Web Server** | Apache | 2.4 |
| **Frontend Framework** | Bootstrap | 3.x |
| **JavaScript Library** | jQuery | 1.x |
| **CLI Tool** | Drush | 8.x |

---

## Custom Modules Created

I developed 4 custom modules from scratch, demonstrating proficiency in Drupal module development, database design, and software architecture.

### 1. Progress Tracker Module ⭐ (Flagship Module)

**Location:** `/sites/all/modules/custom/progress_tracker`  
**Lines of Code:** ~2,000 lines  
**Complexity:** High

**Purpose:** A comprehensive milestone-based progress tracking system with visual timelines, automated notifications, and complete audit trails.

**Features:**
- Visual timeline display with status icons
- Progress bars showing completion percentage (0-100%)
- Status tracking (Not Started, In Progress, Completed, Overdue)
- Priority levels (Low, Medium, High, Critical)
- 10 pre-configured milestone templates
- Email notifications for assignments and overdue items
- Complete audit trail of all progress updates
- Real-time statistics calculation

**Database Tables:**
1. `progress_milestones` - Template definitions
2. `progress_milestone_assignments` - Assigned milestones
3. `progress_updates` - Complete audit trail

**Key Functions:**
- `progress_tracker_get_startup_stats()` - Calculates real-time statistics
- `progress_tracker_load_startup_milestones()` - Retrieves milestones
- `progress_tracker_save_assignment()` - Creates assignments
- `progress_tracker_update_progress()` - Records updates

---

### 2. Mentor Matching Module

**Location:** `/sites/all/modules/custom/mentor_matching`  
**Lines of Code:** ~400 lines

**Purpose:** Manages mentor-startup relationships with assignment tracking and history.

**Features:**
- Assign mentors to startups
- Track assignment history
- Email notifications
- Admin management interface

**Database Table:** `mentor_matching_history`

---

### 3. Startup Scorecard Module

**Location:** `/sites/all/modules/custom/startup_scorecard`  
**Lines of Code:** ~500 lines

**Purpose:** Multi-criteria evaluation system for startups.

**Features:**
- 5 scoring criteria (Team, Market, Product, Business Model, Traction)
- Scale: 1-10 for each criterion
- Automatic aggregate calculation
- Historical tracking
- Mentor comments

**Database Table:** `startup_scorecard`

---

### 4. Feedback Entity Module (Custom Entity)

**Location:** `/sites/all/modules/custom/feedback_entity`  
**Lines of Code:** ~200 lines  
**Complexity:** Medium-High

**Purpose:** Custom entity for collecting structured feedback from stakeholders.

**Why a Custom Entity?**

I built this as a custom entity using Entity API instead of a content type because:
- Represents data, not content (no need for revision system)
- Simpler, more efficient structure
- Better performance (fewer database joins)
- Demonstrates advanced Drupal API knowledge

**Features:**
- Custom entity built from scratch using Entity API
- Four feedback types (Mentor Session, Milestone, Resource, General)
- 5-star rating system
- Admin moderation interface
- Status tracking (New, Reviewed, Archived)

**Database Table:** `feedback` (custom entity table)

**Entity Definition:**
```php
'feedback' => array(
  'label' => t('Feedback'),
  'entity class' => 'Entity',
  'controller class' => 'EntityAPIController',
  'base table' => 'feedback',
  'entity keys' => array('id' => 'feedback_id'),
)
```

**CRUD Operations:**

**Create:**
```php
$feedback = entity_create('feedback', array(
  'feedback_type' => 'mentor',
  'rating' => 5,
  'comment' => 'Great session!',
));
entity_save('feedback', $feedback);
```

**Read:**
```php
$feedback = entity_load_single('feedback', $id);
```

**Update:**
```php
$feedback->status = 'reviewed';
entity_save('feedback', $feedback);
```

**Delete:**
```php
entity_delete('feedback', $id);
```

**Automated Testing:**
- Test Suite: FeedbackEntitySimpleTest
- 3 test methods
- 56 assertions
- 100% pass rate

---

## Custom Entity Module - Detailed Explanation

### What is a Custom Entity?

In Drupal, an entity is a data structure representing a distinct object. Built-in entities include Node, User, Taxonomy Term, and Comment.

A **custom entity** is defined from scratch to represent application-specific data.

### When to Use Custom Entity vs. Content Type

**Use Content Type (Node) When:**
- Need revision tracking
- Content workflows important
- Comments required
- SEO and URLs matter

**Use Custom Entity When:**
- Pure data (not "content")
- Don't need node overhead
- Want better performance
- Need full structural control

### Feedback Entity Implementation

**Architecture:**
```
Entity API Framework
        ↓
Entity Definition (hook_entity_info)
        ↓
Database Table (feedback)
        ↓
CRUD Controllers
        ↓
Access Control
        ↓
Forms & UI
```

**Database Schema:**
```sql
feedback table:
- feedback_id (Primary Key)
- feedback_type (mentor/milestone/resource/general)
- rating (1-5 stars)
- comment (detailed feedback)
- submitted_by (user ID)
- created (timestamp)
- status (new/reviewed/archived)
```

**Benefits:**
- Clean, simple structure
- Fast single-table queries
- Full control over functionality
- Professional implementation

---

## Database Design

### Tables Overview

6 custom database tables with proper normalization and indexing:

1. **mentor_matching_history** - Mentor assignments
2. **startup_scorecard** - Evaluation scores
3. **progress_milestones** - Milestone templates
4. **progress_milestone_assignments** - Assigned milestones
5. **progress_updates** - Audit trail
6. **feedback** - Custom entity

### Design Principles

- **Normalization:** Third Normal Form (3NF)
- **Performance:** Strategic indexing
- **Scalability:** Supports 1000+ startups
- **Maintainability:** Clear naming conventions

---

## Security Implementation

### Access Control

**Role-Based Permissions:**
- 5 distinct roles with specific permissions
- Access callbacks on every page
- Data ownership verification

**Permission Matrix:**

| Role | Create | Edit All | Assign Mentor | Submit Score | Reports |
|------|--------|----------|---------------|--------------|---------|
| Admin | ✅ | ✅ | ✅ | ✅ | ✅ |
| Manager | ✅ | ✅ | ✅ | ✅ | ✅ |
| Mentor | ❌ | Assigned | ❌ | ✅ | ✅ |
| Founder | ✅ | Own Only | ❌ | ❌ | Own |
| Investor | ❌ | ❌ | ❌ | ❌ | Funded |

### Security Measures

- **SQL Injection Prevention:** Parameterized queries
- **XSS Protection:** Output sanitization
- **CSRF Protection:** Form tokens
- **Input Validation:** Server-side validation

---

## Testing & Quality Assurance

**Framework:** Drupal SimpleTest

**Test Suite:** FeedbackEntitySimpleTest
- Test Methods: 3
- Total Assertions: 56
- Pass Rate: 100%
- Failures: 0

**Coverage:**
- Functional testing
- Integration testing
- Security testing
- CRUD operations

---

## Installation Instructions

### Prerequisites
- Apache 2.4+
- PHP 7.4
- MySQL 5.7+
- Drush 8.x

### Quick Setup

**1. Import Database:**
```bash
mysql -u root -p sims_db < sims_database.sql
```

**2. Configure:**
- Update `sites/default/settings.php` with database credentials

**3. Set Permissions:**
```bash
chmod 644 sites/default/settings.php
chmod 755 sites/default/files
```

**4. Clear Cache:**
```bash
drush cc all
```

**5. Login:**
- URL: http://localhost/drupal7
- See CREDENTIALS.txt for login details

---

## Project Structure
```
drupal7/
├── sites/all/modules/custom/
│   ├── feedback_entity/
│   │   ├── feedback_entity.info
│   │   ├── feedback_entity.module
│   │   ├── feedback_entity.install
│   │   └── tests/
│   ├── mentor_matching/
│   ├── progress_tracker/
│   └── startup_scorecard/
└── sites/default/
    ├── settings.php
    └── files/
```

---

## Technical Achievements

- ✅ 3,500+ lines of custom code
- ✅ 4 custom modules from scratch
- ✅ 6 normalized database tables
- ✅ 5 role-specific dashboards
- ✅ 8 workflow automation rules
- ✅ 1 custom entity (Entity API)
- ✅ 56 test assertions (100% pass)
- ✅ Production-ready quality

---

## Future Enhancements

- AI-powered mentor matching
- Advanced analytics dashboard
- Document management system
- Mobile application
- RESTful API
- Multi-language support

---

## Credits

**Developer:** Bikee Prajapati  
**Training Program:** Associate Software Engineer (PHP/Drupal)  
**Duration:** 2-3 weeks  
**Completion:** March 2026

---

## Contact

**Email:** bikeeprajapati1@gmail.com  
**GitHub:** https://github.com/bikeeprajapati/Drupal_Tranee  
**LinkedIn:** https://www.linkedin.com/in/bikee-prajapati9898/

---

## License

GPL-2.0 - Educational/Portfolio Project

---

**Last Updated:** March 6, 2026