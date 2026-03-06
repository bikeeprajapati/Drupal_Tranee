# 🚀 SIMS - Startup Incubator Management System

> A comprehensive Drupal 7 web application for managing startup incubation programs, built during technical training.

[![Drupal](https://img.shields.io/badge/Drupal-7.x-0678BE?style=flat-square&logo=drupal)](https://www.drupal.org/)
[![PHP](https://img.shields.io/badge/PHP-7.4-777BB4?style=flat-square&logo=php)](https://www.php.net/)
[![MySQL](https://img.shields.io/badge/MySQL-5.7-4479A1?style=flat-square&logo=mysql)](https://www.mysql.com/)
[![Bootstrap](https://img.shields.io/badge/Bootstrap-3.x-7952B3?style=flat-square&logo=bootstrap)](https://getbootstrap.com/)

---

## 📖 Project Overview

**SIMS (Startup Incubator Management System)** is a full-stack web application that digitizes and automates the entire startup incubation lifecycle. It replaces manual processes (Excel, emails, WhatsApp) with a structured, role-based platform.

### 🎯 Problem Statement

Startup incubators face challenges:
- ❌ Manual tracking of startup applications
- ❌ Inefficient mentor-startup matching  
- ❌ Lack of structured progress monitoring
- ❌ Poor visibility for investors
- ❌ No centralized data or reporting

### ✅ Solution

A web-based platform providing:
- ✅ Automated submission and screening workflows
- ✅ Intelligent mentor assignment system
- ✅ Visual progress tracking with milestones
- ✅ Multi-role dashboards with real-time analytics
- ✅ Scorecard evaluation framework
- ✅ Email notifications and workflow automation
- ✅ Comprehensive reporting and feedback system

---

## 🌟 Key Features

### 🎨 Multi-Role Dashboard System
- **Admin Dashboard** - Portfolio overview, screening queue, analytics
- **Founder Dashboard** - Startup progress, mentor communication, milestones
- **Mentor Dashboard** - Assigned startups, scorecard submission, progress monitoring
- **Investor Dashboard** - Funded startups, ROI metrics, portfolio filtering

### 📊 Progress Tracker Module ⭐ (2,000+ lines)
- Visual timeline with milestone tracking
- Status indicators (Not Started, In Progress, Completed, Overdue)
- Progress bars with completion percentages
- Priority levels (Low, Medium, High, Critical)
- Email notifications for overdue milestones
- Complete audit trail for all updates
- 10 pre-configured milestone templates

### 🤝 Mentor Matching System
- Manual and automated mentor assignment
- Mentor-startup relationship tracking
- Assignment history and audit logs
- Email notifications on assignment

### 📝 Scorecard Evaluation
- Multi-criteria rating (Team, Market, Product, Business Model, Traction)
- Aggregate scoring (1-10 scale)
- Historical scorecard tracking
- Mentor feedback and comments

### 💬 Feedback Entity (Custom Drupal Entity)
- Custom entity built from scratch (not node-based)
- Feedback types: Mentor Session, Milestone, Resource, General
- 5-star rating system
- Admin moderation interface

### 🔔 Workflow Automation
- 8 configured Rules for automation
- Email notifications (status changes, assignments, deadlines)
- Role-based login redirects
- Automated status transitions

---

## 🏗 Technical Architecture

### Technology Stack

| Component | Technology | Version |
|-----------|-----------|---------|
| **CMS Framework** | Drupal | 7.98 |
| **Backend** | PHP | 7.4 |
| **Database** | MySQL | 5.7 |
| **Frontend** | Bootstrap | 3.x |
| **Styling** | Custom CSS3 | Gradients, Animations |
| **JavaScript** | jQuery | (Drupal core) |
| **CLI Tool** | Drush | 8.x |

### Contrib Modules
- Views 3.x - Dynamic content displays
- Rules 2.x - Workflow automation  
- Entity API 1.x - Custom entity foundation
- CTools 1.x - Tools for modules
- Date 2.x - Date field handling

---

## 📦 Custom Modules

### 1. progress_tracker (Flagship - 2,000 lines)
**Location:** `/sites/all/modules/custom/progress_tracker`

**Features:**
- Visual timeline with status icons
- Progress bars (0-100%)
- 10 default milestone templates
- Priority levels
- Overdue detection
- Email notifications
- Statistics calculation
- Update history (audit trail)

**Database Schema:**
- `progress_milestones` - Template definitions
- `progress_milestone_assignments` - Assigned milestones
- `progress_updates` - Audit trail

### 2. mentor_matching
**Location:** `/sites/all/modules/custom/mentor_matching`

**Features:**
- Assign mentors to startups
- Track assignment history
- Query mentor's assigned startups
- Email notifications

### 3. startup_scorecard
**Location:** `/sites/all/modules/custom/startup_scorecard`

**Features:**
- 5 scoring criteria (1-10 scale)
- Aggregate score calculation
- Historical tracking
- Mentor comments

### 4. feedback_entity
**Location:** `/sites/all/modules/custom/feedback_entity`

**Features:**
- Custom entity (Entity API)
- CRUD operations
- Rating system (1-5 stars)
- Admin moderation
- Automated testing

---

## 🗄 Database Design

### Content Types
- **startup_idea** - Main content type with 12+ fields

### Custom Tables (6 total)
1. `mentor_matching_history`
2. `startup_scorecard`
3. `progress_milestones`
4. `progress_milestone_assignments`
5. `progress_updates`
6. `feedback`

### User Roles (5)
- Administrator
- Incubator Manager
- Mentor
- Founder
- Investor

---

## 🔒 Security & Access Control

### Security Features
- Role-based access control (RBAC)
- Permission-based menu access
- Data ownership validation
- SQL injection prevention (parameterized queries)
- XSS protection (check_plain, filter_xss)
- CSRF protection (Drupal form tokens)

### Permissions Matrix

| Role | Create Startup | Edit All | Assign Mentor | Submit Score | View Reports |
|------|---------------|----------|---------------|--------------|--------------|
| Admin | ✅ | ✅ | ✅ | ✅ | ✅ |
| Manager | ✅ | ✅ | ✅ | ✅ | ✅ |
| Mentor | ❌ | Assigned only | ❌ | ✅ | ✅ |
| Founder | ✅ | Own only | ❌ | ❌ | Own only |
| Investor | ❌ | ❌ | ❌ | ❌ | Funded only |

---

## 🧪 Testing

### Automated Testing
**Framework:** Drupal SimpleTest

**Test Suite:** `FeedbackEntitySimpleTest`
- 3 test methods
- 56 assertions
- 100% pass rate
- Test duration: 56 seconds

**Coverage:**
- Form accessibility
- Data persistence
- Admin interface
- Permission validation
- Entity CRUD operations

**Run Tests:**
```bash
drush test-run "Feedback Entity - Basic Test"
```

Or via web UI: `/admin/config/development/testing`

---

## 📊 Project Statistics

| Metric | Value |
|--------|-------|
| **Total Lines of Code** | ~3,500+ |
| **Custom Modules** | 4 |
| **Database Tables** | 6 custom + Drupal core |
| **Views/Dashboards** | 5 |
| **Workflow Rules** | 8 |
| **Content Types** | 1 main (startup_idea) |
| **User Roles** | 5 |
| **Test Assertions** | 56 (100% pass) |
| **Development Time** | 2-3 weeks |
| **Files Created** | 30+ |

---

## 🎨 UI/UX Design

### Design Philosophy
- **Modern Gradient UI** - Purple gradient (#667eea → #764ba2)
- **Glassmorphism** - Backdrop blur effects
- **Color-coded Status** - Visual indicators
- **Responsive Design** - Mobile-first approach
- **Smooth Animations** - Hover effects and transitions

### Status Color Coding
- 🟢 **Funded** - Green
- 🔵 **Selected** - Blue
- 🟡 **Screening** - Yellow
- 🔴 **Rejected** - Red
- ⚪ **Submitted** - Purple

---

## 🚀 Installation

### Prerequisites
- Apache 2.4+
- PHP 7.2+ (7.4 recommended)
- MySQL 5.7+ or MariaDB 10.3+
- Drush 8.x (optional)

### Quick Install
```bash
# Clone repository
git clone https://github.com/bikeeprajapati/Drupal_Tranee.git
cd Drupal_Tranee

# Configure database
cp sites/default/default.settings.php sites/default/settings.php
# Edit settings.php with your database credentials

# Install contrib modules
drush dl views rules entity ctools date
drush en views rules entity ctools date -y

# Enable custom modules
drush en mentor_matching startup_scorecard progress_tracker feedback_entity -y

# Clear cache
drush cc all
```

### Full Installation Guide
See [INSTALL.txt](INSTALL.txt) for detailed setup instructions.

---

## 🎯 Learning Outcomes

### Technical Skills Demonstrated

**Backend Development:**
- Custom Drupal module development
- Database schema design and normalization
- Entity API implementation
- Form API (validation, submission handlers)
- Hook system implementation
- CRUD operations
- SQL query optimization

**Frontend Development:**
- Responsive UI design (Bootstrap)
- Custom CSS3 (gradients, animations)
- JavaScript/jQuery
- AJAX interactions
- UX design principles

**System Design:**
- Multi-tier architecture
- Role-based access control (RBAC)
- Workflow automation (Rules)
- Email notification system
- RESTful principles

**Testing & Quality:**
- Unit testing (SimpleTest)
- Functional testing
- Security testing
- Test-driven development

---

## 📂 Project Structure
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
│   │   ├── progress_tracker.info
│   │   ├── progress_tracker.module
│   │   ├── progress_tracker.install
│   │   ├── progress_tracker.admin.inc
│   │   ├── progress_tracker.pages.inc
│   │   ├── progress_tracker.css
│   │   └── progress_tracker.js
│   └── startup_scorecard/
└── sites/all/themes/bootstrap/
```

---

## 🎬 Demo

**Live Instance:** http://localhost/drupal7

**Test Credentials:**
- Admin: `admin` / `[password]`
- Founder: `founder1` / `test123`
- Mentor: `mentor1` / `test123`

**Key Pages:**
- Admin Dashboard: `/admin/dashboard`
- Startup Listing: `/startup-dashboard`
- Progress Tracker: `/node/[nid]/progress`
- Feedback Form: `/feedback/submit`

---

## 🚀 Future Enhancements

### Planned Features
- [ ] AI-powered mentor matching
- [ ] Advanced analytics with charts
- [ ] Document management system
- [ ] Calendar integration (Google Calendar)
- [ ] Mobile app (React Native)
- [ ] RESTful API for integrations
- [ ] Multi-language support (i18n)

---

## 💼 Project Highlights

### What Makes This Special

**1. Production-Ready Quality**
- Proper error handling
- Input validation
- Security best practices
- Comprehensive testing

**2. Full-Stack Development**
- Database design
- Backend logic (PHP/Drupal)
- Frontend (HTML/CSS/JS)
- System integration

**3. Complex Business Logic**
- Multi-role permissions
- Workflow automation
- Custom entity development
- Audit trails

**4. Modern UI/UX**
- Custom gradient design
- Smooth animations
- Responsive mobile layout

**5. Enterprise Features**
- Role-based dashboards
- Email notifications
- Progress tracking
- Reporting & analytics

---

## 📝 Documentation

- **README.md** - This file
- **Code Comments** - Inline documentation throughout
- **Test Documentation** - In `/tests/` directories
- **API Documentation** - Function headers with parameters

---

## 👨‍💻 Author

**Bikee Prajapati**
- GitHub: [@bikeeprajapati](https://github.com/bikeeprajapati)
- Email: your.email@example.com
- LinkedIn: [Your LinkedIn]

---

## 📄 License

GPL-2.0 License - See [LICENSE.txt](LICENSE.txt)

---

## 🙏 Acknowledgments

- Drupal Community
- Bootstrap Team
- Training mentors and peers

---

**⭐ This project demonstrates:**
- Full-stack web development
- Problem-solving abilities
- Code quality standards
- Self-learning initiative
- Project completion capability

**Built with ❤️ during technical training program**

---

*Last Updated: March 2026*
