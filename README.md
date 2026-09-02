# CampusCare --- Campus Complaint & Maintenance Management System

## Submitted To

**Hafsa Binte Kibria**\
**Assistant Professor**\
**Department of Electrical and Computer Engineering**\
**Rajshahi University of Engineering & Technology (RUET)**

## Team Members

  ---------------------------------------------------------------------------
                     Name                               ID               Contribution

                     1 Md. Amir                     2510020               Designed and
                       Hamza                                              implemented the
                                                                          User/Complainer
                                                                          Portal, complaint
                                                                          submission and
                                                                          tracking
                                                                          features, and
                                                                          persistent
                                                                          complaint data
                                                                          management.

                     2 Alif Al                     2510019                Designed and
                       Hasan                                              implemented the
                                                                          Management
                                                                          Portal, including
                                                                          complaint
                                                                          searching,
                                                                          updating,
                                                                          assignment, and
                                                                          status
                                                                          management.

                     3 Md. Omar Faruk Shifat       2510017                Worked on file
                                                                          handling,
                                                                          resolved
                                                                          complaint
                                                                          history,
                                                                          complaint
                                                                          statistics,
                                                                          testing,
                                                                          debugging, and
                                                                          overall system
                                                                          integration.
  ---------------------------------------------------------------------------
## Video Presentation:
[Click Here]()

---

## Project Title

**CampusCare --- Campus Complaint & Maintenance Management System**

---

## Project Objectives

-   To develop a practical complaint management system using C.
-   To make campus complaint submission easier.
-   To allow users to track their complaints.
-   To provide management with an organized way to handle complaints.
-   To maintain complaint records using file handling.
-   To apply fundamental C programming concepts to a real-world
    application.

---

## Introduction

CampusCare is a C-based console application designed to manage
complaints and maintenance problems within a university campus.

Students and staff can submit complaints related to electrical problems,
plumbing, cleaning, internet, furniture, and other campus facilities.
Management can then review, assign, update, and resolve these
complaints.

The system makes the complaint management process more organized,
efficient, and easy to track.

---

## Project Description

CampusCare consists of two main portals.

### User / Complainer Portal

Users can:

-   Submit new complaints
-   Select complaint categories
-   Set complaint priority
-   View their submitted complaints
-   Track complaint status
-   View resolved complaint history

### Management Portal

Management can:

-   View all active complaints
-   Search for complaints
-   Update complaint information
-   Assign maintenance staff
-   Delete complaints
-   View pending complaints
-   Resolve complaints
-   View resolved complaint history
-   View complaint statistics

Complaint data is automatically saved, so users do not need to manually
save their information.

---

## Features

### User Features

-   Complaint submission
-   Complaint tracking
-   Complaint status viewing
-   Resolved complaint history
-   Automatic data saving

### Management Features

-   Secure management login
-   Complaint searching
-   Complaint updating
-   Maintenance staff assignment
-   Complaint deletion
-   Complaint resolution
-   Complaint statistics
-   Resolved complaint history

### Technical Features

-   Structures
-   Unions
-   Enumerations
-   Functions
-   Pointers
-   Dynamic memory allocation
-   File handling
-   String handling
-   Recursion
-   Input validation

## Complaint Categories

-   Electrical
-   Plumbing
-   Cleaning
-   Internet
-   Furniture
-   Other

## Priority Levels

-   Low
-   Medium
-   High
-   Urgent

## Complaint Status

-   **Pending**
-   **In Progress**
-   **Resolved**
-   **Rejected**

---

## How to Use

Copy the **CampusCare** project folder to your local machine. Open the
project in a C development environment such as **Code::Blocks**, build
or compile the `campuscare.c` source file, and then run the program.

### User / Complainer Portal

1.  Select **User / Complainer Portal** from the main menu.
2.  Submit a new complaint.
3.  Enter the required complaint information.
4.  Select the appropriate category and priority.
5.  View your submitted complaints.
6.  Track a complaint using its complaint ID.
7.  View resolved complaints from **My Resolved History**.

The complaint information is automatically saved after submission.

### Management Portal

1.  Select **Management Portal** from the main menu.
2.  Enter the management password.
3.  View all active complaints.
4.  Search for a specific complaint.
5.  Update complaint information when necessary.
6.  Assign a maintenance staff member.
7.  View pending complaints.
8.  Resolve a complaint after the problem has been fixed.
9.  View resolved complaint history and statistics.

When a complaint is marked as **Resolved**, it is automatically removed
from the active complaint list and stored in the resolved complaint
history.

## Automatic Data Storage

CampusCare uses text files to permanently store complaint information.

  File                        Purpose
  --------------------------- -----------------------------------
  `complaints.txt`            Stores active complaints
  `resolved_complaints.txt`   Stores resolved complaint history

The system automatically saves data whenever complaints are submitted or
modified. No separate **Save** option is required.

## Complaint Information

Each complaint contains information such as:

-   Complaint ID
-   Complainant name
-   Reporter type
-   Student/Staff ID
-   Location
-   Complaint category
-   Description
-   Priority
-   Status
-   Assigned maintenance staff
-   Date

## Complaint Lifecycle

``` text
Submit Complaint
       ↓
     Pending
       ↓
   In Progress
       ↓
    Resolved
       ↓
Resolved History
```

A complaint may also be **Rejected** or **Deleted** by management when
necessary.

## Technologies Used

-   **Programming Language:** C
-   **Interface:** Console / Command Line
-   **Data Storage:** Text Files
-   **Development Environment:** Code::Blocks / GCC / Other C Compiler

## Project Structure

``` text
CampusCare/
│
├── campuscare.c
├── complaints.txt
├── resolved_complaints.txt
```

## Management Login

The Management Portal is protected by a password.

**Management Password:** `admin123`

> The password is included for academic and demonstration purposes.

---

## Conclusion

CampusCare provides a simple and organized solution for managing campus
complaints and maintenance problems. It allows students and staff to
submit and track complaints while providing management with the tools
required to manage, assign, and resolve them.

The project demonstrates important C programming concepts such as
**structures, unions, enumerations, dynamic memory allocation, file
handling, functions, and recursion**, while applying them to a practical
campus-based system.

------------------------------------------------------------------------

**Developed for academic purposes**\
**Department of Electrical and Computer Engineering**\
**Rajshahi University of Engineering & Technology (RUET)**
