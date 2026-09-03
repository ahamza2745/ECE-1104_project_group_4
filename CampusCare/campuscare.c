#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define DATA_FILE "complaints.txt"
#define RESOLVED_FILE "resolved_complaints.txt"

#define NAME_SIZE 60
#define ID_SIZE 30
#define LOCATION_SIZE 100
#define DESCRIPTION_SIZE 300
#define STAFF_SIZE 60
#define DATE_SIZE 20


typedef enum {
    STUDENT = 1,
    STAFF = 2
} ReporterType;

typedef enum {
    ELECTRICAL = 1,
    PLUMBING = 2,
    CLEANING = 3,
    INTERNET = 4,
    FURNITURE = 5,
    OTHER = 6
} Category;

typedef enum {
    LOW = 1,
    MEDIUM = 2,
    HIGH = 3,
    URGENT = 4
} Priority;

typedef enum {
    PENDING = 1,
    IN_PROGRESS = 2,
    RESOLVED = 3,
    REJECTED = 4
} Status;

typedef union {
    char studentId[ID_SIZE];
    char staffId[ID_SIZE];
} ReporterID;

typedef struct {
    int complaintId;
    char name[NAME_SIZE];
    ReporterType reporterType;
    ReporterID reporterId;
    char location[LOCATION_SIZE];
    Category category;
    char description[DESCRIPTION_SIZE];
    Priority priority;
    Status status;
    char assignedTo[STAFF_SIZE];
    char date[DATE_SIZE];
} Complaint;

typedef struct {
    Complaint *items;
    int size;
    int capacity;
} ComplaintList;


void clearInput(void);
void trimNewline(char *text);
void readString(const char *message, char *text, int size);
int readInt(const char *message, int min, int max);

const char *categoryName(Category category);
const char *priorityName(Priority priority);
const char *statusName(Status status);

void getToday(char *date);
void getDataFilePath(char *path, int size);
void getResolvedFilePath(char *path, int size);

void initializeList(ComplaintList *list);
void freeList(ComplaintList *list);
void addComplaintToList(ComplaintList *list, Complaint complaint);
void removeComplaint(ComplaintList *list, int index);
int findComplaint(const ComplaintList *list, int id);

int getNextComplaintId(const ComplaintList *list);

int saveData(const ComplaintList *list);
int loadData(ComplaintList *list);
int archiveResolvedComplaint(const Complaint *complaint);
int countResolvedForUser(ReporterType type, const char *id);

void printComplaint(const Complaint *complaint);
void printComplaintSummary(const Complaint *complaint);

void userPortal(ComplaintList *list);
void managementPortal(ComplaintList *list);

void submitComplaint(ComplaintList *list, const char *name,
                      ReporterType type, const char *userId);
void viewMyComplaints(const ComplaintList *list, ReporterType type,
                      const char *userId);
void trackMyComplaint(const ComplaintList *list, ReporterType type,
                      const char *userId);
void viewMyResolvedHistory(ReporterType type, const char *userId);

void viewAllComplaints(const ComplaintList *list);
void searchComplaint(const ComplaintList *list);
void updateComplaint(ComplaintList *list);
void assignStaff(ComplaintList *list);
void deleteComplaint(ComplaintList *list);
void viewPendingComplaints(const ComplaintList *list);
void viewResolvedHistory(void);
void showStatistics(const ComplaintList *list);
void displayComplaintIdsRecursively(const ComplaintList *list, int index);

int managementLogin(void);

/* SHARED / INTEGRATED SECTION */

int main(void)
{
    ComplaintList list;

    initializeList(&list);

    loadData(&list);

    while (1) {
        int choice;

        printf("\n===============================================\n");
        printf("                  CAMPUSCARE\n");
        printf("      CAMPUS COMPLAINT MANAGEMENT SYSTEM\n");
        printf("===============================================\n");
        printf("1. User / Complainer Portal\n");
        printf("2. Management Portal\n");
        printf("0. Exit\n");
        printf("-----------------------------------------------\n");

        choice = readInt("Enter your choice: ", 0, 2);

        if (choice == 1) {
            userPortal(&list);
        } else if (choice == 2) {
            if (managementLogin())
                managementPortal(&list);
        } else {

            saveData(&list);
            printf("\nThank you for using CampusCare.\n");
            break;
        }
    }

    freeList(&list);
    return 0;
}

void clearInput(void)
{
    int ch;

    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}

void trimNewline(char *text)
{
    size_t length;

    if (text == NULL)
        return;

    length = strlen(text);

    while (length > 0 &&
           (text[length - 1] == '\n' || text[length - 1] == '\r')) {
        text[length - 1] = '\0';
        length--;
    }
}

void readString(const char *message, char *text, int size)
{
    while (1) {
        printf("%s", message);

        if (fgets(text, size, stdin) == NULL) {
            text[0] = '\0';
            return;
        }

        if (strchr(text, '\n') == NULL)
            clearInput();

        trimNewline(text);

        if (strlen(text) == 0) {
            printf("This field cannot be empty.\n");
            continue;
        }

        if (strchr(text, '|') != NULL) {
            printf("Please do not use the '|' character.\n");
            continue;
        }

        return;
    }
}

int readInt(const char *message, int min, int max)
{
    char line[100];
    char extra;
    int value;

    while (1) {
        printf("%s", message);

        if (fgets(line, sizeof(line), stdin) == NULL)
            continue;

        if (sscanf(line, " %d %c", &value, &extra) == 1 &&
            value >= min && value <= max) {
            return value;
        }

        printf("Invalid input. Please enter a number from %d to %d.\n",
               min, max);
    }
}

const char *categoryName(Category category)
{
    switch (category) {
        case ELECTRICAL: return "Electrical";
        case PLUMBING:   return "Plumbing";
        case CLEANING:   return "Cleaning";
        case INTERNET:   return "Internet";
        case FURNITURE:  return "Furniture";
        default:         return "Other";
    }
}

const char *priorityName(Priority priority)
{
    switch (priority) {
        case LOW:    return "Low";
        case MEDIUM: return "Medium";
        case HIGH:   return "High";
        case URGENT: return "Urgent";
        default:     return "Unknown";
    }
}

const char *statusName(Status status)
{
    switch (status) {
        case PENDING:     return "Pending";
        case IN_PROGRESS: return "In Progress";
        case RESOLVED:    return "Resolved";
        case REJECTED:    return "Rejected";
        default:          return "Unknown";
    }
}

void printComplaintSummary(const Complaint *complaint)
{
    printf("\n-----------------------------------------------\n");
    printf("Complaint ID : %d\n", complaint->complaintId);
    printf("Name         : %s\n", complaint->name);
    printf("Location     : %s\n", complaint->location);
    printf("Category     : %s\n", categoryName(complaint->category));
    printf("Priority     : %s\n", priorityName(complaint->priority));
    printf("Status       : %s\n", statusName(complaint->status));
    printf("Assigned To  : %s\n",
           strlen(complaint->assignedTo) > 0 ?
           complaint->assignedTo : "Not assigned");
    printf("Date         : %s\n", complaint->date);
}

void printComplaint(const Complaint *complaint)
{
    const char *id;

    if (complaint->reporterType == STUDENT)
        id = complaint->reporterId.studentId;
    else
        id = complaint->reporterId.staffId;

    printComplaintSummary(complaint);

    printf("Reporter ID  : %s\n", id);
    printf("Description  : %s\n", complaint->description);
}

void getToday(char *date)
{
    time_t now;
    struct tm *currentTime;

    time(&now);
    currentTime = localtime(&now);

    if (currentTime != NULL) {
        strftime(date, DATE_SIZE, "%Y-%m-%d", currentTime);
    } else {
        strcpy(date, "Unknown");
    }
}

void getDataFilePath(char *path, int size)
{

    char exePath[MAX_PATH];
    DWORD length;
    char *lastSlash;

    length = GetModuleFileNameA(NULL, exePath, MAX_PATH);

    if (length > 0 && length < MAX_PATH) {
        exePath[length] = '\0';
        lastSlash = strrchr(exePath, '\\');

        if (lastSlash != NULL) {
            *lastSlash = '\0';
            snprintf(path, size, "%s\\%s", exePath, DATA_FILE);
            return;
        }
    }


    snprintf(path, size, "%s", DATA_FILE);
}

void getResolvedFilePath(char *path, int size)
{
#ifdef _WIN32
    char exePath[MAX_PATH];
    DWORD length;
    char *lastSlash;

    length = GetModuleFileNameA(NULL, exePath, MAX_PATH);

    if (length > 0 && length < MAX_PATH) {
        exePath[length] = '\0';
        lastSlash = strrchr(exePath, '\\');

        if (lastSlash != NULL) {
            *lastSlash = '\0';
            snprintf(path, size, "%s\\%s", exePath, RESOLVED_FILE);
            return;
        }
    }
#endif

    snprintf(path, size, "%s", RESOLVED_FILE);
}

void initializeList(ComplaintList *list)
{
    list->size = 0;
    list->capacity = 10;
    list->items = malloc(list->capacity * sizeof(Complaint));

    if (list->items == NULL) {
        printf("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
}

void freeList(ComplaintList *list)
{
    free(list->items);
    list->items = NULL;
    list->size = 0;
    list->capacity = 0;
}

void addComplaintToList(ComplaintList *list, Complaint complaint)
{
    Complaint *newItems;

    if (list->size >= list->capacity) {
        list->capacity *= 2;

        newItems = realloc(list->items,
                           list->capacity * sizeof(Complaint));

        if (newItems == NULL) {
            printf("Unable to add complaint. Memory allocation failed.\n");
            exit(EXIT_FAILURE);
        }

        list->items = newItems;
    }

    list->items[list->size] = complaint;
    list->size++;
}

void removeComplaint(ComplaintList *list, int index)
{
    int i;

    if (index < 0 || index >= list->size)
        return;

    for (i = index; i < list->size - 1; i++)
        list->items[i] = list->items[i + 1];

    list->size--;
}

int findComplaint(const ComplaintList *list, int id)
{
    int i;

    for (i = 0; i < list->size; i++) {
        if (list->items[i].complaintId == id)
            return i;
    }

    return -1;
}

int getNextComplaintId(const ComplaintList *list)
{
    int highest = 1000;
    int i;

    for (i = 0; i < list->size; i++) {
        if (list->items[i].complaintId > highest)
            highest = list->items[i].complaintId;
    }

    return highest + 1;
}

/*PART 1: USER / COMPLAINER PORTAL
Md. Amir Hamza
Complaint submission, tracking, and persistent active complaint data
*/

int saveData(const ComplaintList *list)
{
    char path[512];
    FILE *file;
    int i;

    getDataFilePath(path, sizeof(path));

    file = fopen(path, "w");

    if (file == NULL) {
        printf("\nWARNING: Could not save data.\n");
        printf("File location: %s\n", path);
        return 0;
    }

    for (i = 0; i < list->size; i++) {
        const Complaint *c = &list->items[i];
        const char *id;

        if (c->reporterType == STUDENT)
            id = c->reporterId.studentId;
        else
            id = c->reporterId.staffId;

        fprintf(file,
                "%d|%s|%d|%s|%s|%d|%s|%d|%d|%s|%s\n",
                c->complaintId,
                c->name,
                c->reporterType,
                id,
                c->location,
                c->category,
                c->description,
                c->priority,
                c->status,
                c->assignedTo,
                c->date);
    }

    fclose(file);

    printf("Data saved successfully.\n");
    return 1;
}

int loadData(ComplaintList *list)
{
    char path[512];
    FILE *file;
    char line[1200];
    int loaded = 0;

    getDataFilePath(path, sizeof(path));

    file = fopen(path, "r");


    if (file == NULL)
        return 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        char *fields[11];
        char *token;
        Complaint c;
        int fieldCount = 0;

        trimNewline(line);

        if (strlen(line) == 0)
            continue;

        token = strtok(line, "|");

        while (token != NULL && fieldCount < 11) {
            fields[fieldCount++] = token;
            token = strtok(NULL, "|");
        }

        if (fieldCount != 11)
            continue;

        memset(&c, 0, sizeof(c));

        c.complaintId = atoi(fields[0]);
        strncpy(c.name, fields[1], NAME_SIZE - 1);
        c.reporterType = (ReporterType)atoi(fields[2]);

        if (c.reporterType == STUDENT)
            strncpy(c.reporterId.studentId, fields[3], ID_SIZE - 1);
        else
            strncpy(c.reporterId.staffId, fields[3], ID_SIZE - 1);

        strncpy(c.location, fields[4], LOCATION_SIZE - 1);
        c.category = (Category)atoi(fields[5]);
        strncpy(c.description, fields[6], DESCRIPTION_SIZE - 1);
        c.priority = (Priority)atoi(fields[7]);
        c.status = (Status)atoi(fields[8]);
        strncpy(c.assignedTo, fields[9], STAFF_SIZE - 1);
        strncpy(c.date, fields[10], DATE_SIZE - 1);

        addComplaintToList(list, c);
        loaded++;
    }

    fclose(file);

    return loaded;
}

void userPortal(ComplaintList *list)
{
    char name[NAME_SIZE];
    char userId[ID_SIZE];
    ReporterType type;
    int choice;

    printf("\n===============================================\n");
    printf("                 USER PORTAL\n");
    printf("===============================================\n");

    readString("Enter your name: ", name, sizeof(name));

    printf("\n1. Student\n");
    printf("2. Staff\n");
    type = (ReporterType)readInt("Choose your type: ", 1, 2);

    readString(type == STUDENT ?
               "Enter your Student ID: " :
               "Enter your Staff ID: ",
               userId, sizeof(userId));

    while (1) {
        printf("\n===============================================\n");
        printf("              USER PORTAL\n");
        printf("              Welcome, %s\n", name);
        printf("===============================================\n");
        printf("1. Submit Complaint\n");
        printf("2. View My Complaints\n");
        printf("3. Track My Complaint\n");
        printf("4. My Resolved History\n");
        printf("0. Logout\n");
        printf("-----------------------------------------------\n");
        printf("All complaint changes are saved automatically.\n");

        choice = readInt("Enter your choice: ", 0, 4);

        switch (choice) {
            case 1:
                submitComplaint(list, name, type, userId);
                break;

            case 2:
                viewMyComplaints(list, type, userId);
                break;

            case 3:
                trackMyComplaint(list, type, userId);
                break;

            case 4:
                viewMyResolvedHistory(type, userId);
                break;

            case 0:
                printf("Logged out successfully.\n");
                return;
        }
    }
}

void submitComplaint(ComplaintList *list, const char *name,
                      ReporterType type, const char *userId)
{
    Complaint c;

    memset(&c, 0, sizeof(c));

    c.complaintId = getNextComplaintId(list);
    strncpy(c.name, name, NAME_SIZE - 1);
    c.reporterType = type;

    if (type == STUDENT)
        strncpy(c.reporterId.studentId, userId, ID_SIZE - 1);
    else
        strncpy(c.reporterId.staffId, userId, ID_SIZE - 1);

    readString("Enter location: ", c.location, sizeof(c.location));

    printf("\nComplaint Category\n");
    printf("1. Electrical\n");
    printf("2. Plumbing\n");
    printf("3. Cleaning\n");
    printf("4. Internet\n");
    printf("5. Furniture\n");
    printf("6. Other\n");
    c.category = (Category)readInt("Choose category: ", 1, 6);

    readString("Describe the problem: ",
               c.description, sizeof(c.description));

    printf("\nPriority\n");
    printf("1. Low\n");
    printf("2. Medium\n");
    printf("3. High\n");
    printf("4. Urgent\n");
    c.priority = (Priority)readInt("Choose priority: ", 1, 4);

    c.status = PENDING;
    strcpy(c.assignedTo, "Not assigned");
    getToday(c.date);

    addComplaintToList(list, c);


    saveData(list);

    printf("\nComplaint submitted successfully!\n");
    printf("Your Complaint ID is: %d\n", c.complaintId);
    printf("Keep this ID for tracking.\n");
}

void viewMyComplaints(const ComplaintList *list, ReporterType type,
                      const char *userId)
{
    int i;
    int found = 0;

    printf("\n========== MY ACTIVE COMPLAINTS ==========\n");

    for (i = 0; i < list->size; i++) {
        const Complaint *c = &list->items[i];
        const char *id;

        if (c->reporterType == STUDENT)
            id = c->reporterId.studentId;
        else
            id = c->reporterId.staffId;

        if (c->reporterType == type && strcmp(id, userId) == 0) {
            printComplaint(c);
            found = 1;
        }
    }

    if (!found)
        printf("You have no active complaints.\n");
}

void trackMyComplaint(const ComplaintList *list, ReporterType type,
                      const char *userId)
{
    int id;
    int index;
    const Complaint *c;
    const char *ownerId;

    id = readInt("Enter Complaint ID: ", 1001, 999999);

    index = findComplaint(list, id);

    if (index == -1) {
        printf("Complaint not found in active complaints.\n");
        printf("If it has been resolved, check My Resolved History.\n");
        return;
    }

    c = &list->items[index];

    if (c->reporterType == STUDENT)
        ownerId = c->reporterId.studentId;
    else
        ownerId = c->reporterId.staffId;

    if (c->reporterType != type || strcmp(ownerId, userId) != 0) {
        printf("You can only track your own complaint.\n");
        return;
    }

    printf("\n========== COMPLAINT STATUS ==========\n");
    printComplaint(c);
}

/* MANAGEMENT PORTAL(Alif)*/

int managementLogin(void)
{
    const char correctPassword[] = "admin123";
    char password[50];
    int attempt;

    printf("\n========== MANAGEMENT LOGIN ==========\n");

    for (attempt = 1; attempt <= 3; attempt++) {
        readString("Enter management password: ",
                   password, sizeof(password));

        if (strcmp(password, correctPassword) == 0) {
            printf("Login successful.\n");
            return 1;
        }

        printf("Incorrect password. Attempts left: %d\n",
               3 - attempt);
    }

    printf("Login failed.\n");
    return 0;
}

void managementPortal(ComplaintList *list)
{
    int choice;

    while (1) {
        printf("\n===============================================\n");
        printf("             MANAGEMENT PORTAL\n");
        printf("              CampusCare Admin\n");
        printf("===============================================\n");
        printf("1. View All Complaints\n");
        printf("2. Search Complaint\n");
        printf("3. Update Complaint\n");
        printf("4. Assign Maintenance Staff\n");
        printf("5. Delete Complaint\n");
        printf("6. View Pending Complaints\n");
        printf("7. View Resolved History\n");
        printf("8. Complaint Statistics\n");
        printf("0. Logout\n");
        printf("-----------------------------------------------\n");
        printf("All changes are saved automatically.\n");

        choice = readInt("Enter your choice: ", 0, 8);

        switch (choice) {
            case 1:
                viewAllComplaints(list);
                break;

            case 2:
                searchComplaint(list);
                break;

            case 3:
                updateComplaint(list);
                break;

            case 4:
                assignStaff(list);
                break;

            case 5:
                deleteComplaint(list);
                break;

            case 6:
                viewPendingComplaints(list);
                break;

            case 7:
                viewResolvedHistory();
                break;

            case 8:
                showStatistics(list);
                break;

            case 0:
                printf("Management logged out.\n");
                return;
        }
    }
}

void viewAllComplaints(const ComplaintList *list)
{
    int i;

    printf("\n========== ALL ACTIVE COMPLAINTS ==========\n");

    if (list->size == 0) {
        printf("There are no active complaints.\n");
        return;
    }

    for (i = 0; i < list->size; i++)
        printComplaint(&list->items[i]);

    printf("\nTotal active complaints: %d\n", list->size);
}

void searchComplaint(const ComplaintList *list)
{
    int id;
    int index;

    id = readInt("Enter Complaint ID: ", 1001, 999999);
    index = findComplaint(list, id);

    if (index == -1) {
        printf("Complaint not found in active complaints.\n");
        printf("Check View Resolved History if necessary.\n");
        return;
    }

    printf("\n========== SEARCH RESULT ==========\n");
    printComplaint(&list->items[index]);
}

void updateComplaint(ComplaintList *list)
{
    int id;
    int index;
    int choice;
    Complaint *c;

    id = readInt("Enter Complaint ID: ", 1001, 999999);
    index = findComplaint(list, id);

    if (index == -1) {
        printf("Complaint not found.\n");
        return;
    }

    c = &list->items[index];

    printf("\nCurrent complaint:\n");
    printComplaint(c);

    printf("\n1. Change Priority\n");
    printf("2. Change Status\n");
    printf("3. Change Description\n");
    printf("4. Change Location\n");
    printf("0. Cancel\n");

    choice = readInt("Choose option: ", 0, 4);

    switch (choice) {
        case 1:
            printf("\n1. Low\n");
            printf("2. Medium\n");
            printf("3. High\n");
            printf("4. Urgent\n");

            c->priority = (Priority)readInt(
                "New priority: ", 1, 4);

            saveData(list);
            printf("Priority updated and saved.\n");
            break;

        case 2:
            printf("\n1. Pending\n");
            printf("2. In Progress\n");
            printf("3. Resolved\n");
            printf("4. Rejected\n");

            {
                Status newStatus =
                    (Status)readInt("New status: ", 1, 4);

                if (newStatus == RESOLVED) {

                    if (!archiveResolvedComplaint(c)) {
                        printf("Complaint was NOT resolved because its history "
                               "could not be saved.\n");
                        return;
                    }

                    removeComplaint(list, index);
                    saveData(list);

                    printf("\nComplaint #%d resolved successfully.\n", id);
                    printf("It was moved to resolved complaint history.\n");
                } else {
                    c->status = newStatus;
                    saveData(list);
                    printf("Status updated and saved.\n");
                }
            }
            break;

        case 3:
            readString("New description: ",
                       c->description,
                       sizeof(c->description));

            saveData(list);
            printf("Description updated and saved.\n");
            break;

        case 4:
            readString("New location: ",
                       c->location,
                       sizeof(c->location));

            saveData(list);
            printf("Location updated and saved.\n");
            break;

        case 0:
            printf("Update cancelled.\n");
            break;
    }
}

void assignStaff(ComplaintList *list)
{
    int id;
    int index;
    Complaint *c;

    id = readInt("Enter Complaint ID: ", 1001, 999999);
    index = findComplaint(list, id);

    if (index == -1) {
        printf("Complaint not found.\n");
        return;
    }

    c = &list->items[index];

    readString("Enter maintenance staff name: ",
               c->assignedTo,
               sizeof(c->assignedTo));

    if (c->status == PENDING)
        c->status = IN_PROGRESS;

    saveData(list);

    printf("Maintenance staff assigned successfully.\n");
    printf("Complaint status is now: %s\n",
           statusName(c->status));
}

void deleteComplaint(ComplaintList *list)
{
    int id;
    int index;

    id = readInt("Enter Complaint ID: ", 1001, 999999);
    index = findComplaint(list, id);

    if (index == -1) {
        printf("Complaint not found.\n");
        return;
    }

    printComplaint(&list->items[index]);

    printf("\nThis permanently removes the active complaint.\n");
    printf("Resolved complaints should be closed using Update -> Resolved.\n");

    if (readInt("Type 1 to confirm deletion, 0 to cancel: ", 0, 1) == 1) {
        removeComplaint(list, index);
        saveData(list);
        printf("Complaint deleted and data saved.\n");
    } else {
        printf("Deletion cancelled.\n");
    }
}

void viewPendingComplaints(const ComplaintList *list)
{
    int i;
    int found = 0;

    printf("\n========== PENDING COMPLAINTS ==========\n");

    for (i = 0; i < list->size; i++) {
        if (list->items[i].status == PENDING) {
            printComplaint(&list->items[i]);
            found = 1;
        }
    }

    if (!found)
        printf("There are no pending complaints.\n");
}
/*
FILE HANDLING, RESOLVED HISTORY & STATISTICS
Md. Shifat

 */

int archiveResolvedComplaint(const Complaint *complaint)
{
    char path[512];
    FILE *file;
    const char *id;

    getResolvedFilePath(path, sizeof(path));

    file = fopen(path, "a");

    if (file == NULL) {
        printf("\nWARNING: Could not save resolved complaint history.\n");
        return 0;
    }

    if (complaint->reporterType == STUDENT)
        id = complaint->reporterId.studentId;
    else
        id = complaint->reporterId.staffId;

    fprintf(file,
            "%d|%s|%d|%s|%s|%d|%s|%d|%d|%s|%s\n",
            complaint->complaintId,
            complaint->name,
            complaint->reporterType,
            id,
            complaint->location,
            complaint->category,
            complaint->description,
            complaint->priority,
            RESOLVED,
            complaint->assignedTo,
            complaint->date);

    fclose(file);
    return 1;
}

int countResolvedForUser(ReporterType type, const char *id)
{
    char path[512];
    FILE *file;
    char line[1200];
    int count = 0;

    getResolvedFilePath(path, sizeof(path));

    file = fopen(path, "r");

    if (file == NULL)
        return 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        char *fields[11];
        char *token;
        int n = 0;

        trimNewline(line);

        token = strtok(line, "|");

        while (token != NULL && n < 11) {
            fields[n++] = token;
            token = strtok(NULL, "|");
        }

        if (n == 11 &&
            atoi(fields[2]) == type &&
            strcmp(fields[3], id) == 0) {
            count++;
        }
    }

    fclose(file);
    return count;
}

void viewMyResolvedHistory(ReporterType type, const char *userId)
{
    char path[512];
    FILE *file;
    char line[1200];
    int found = 0;

    getResolvedFilePath(path, sizeof(path));

    file = fopen(path, "r");

    printf("\n========== MY RESOLVED HISTORY ==========\n");

    if (file == NULL) {
        printf("You have no resolved complaints.\n");
        return;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        char *fields[11];
        char *token;
        int n = 0;

        trimNewline(line);

        token = strtok(line, "|");

        while (token != NULL && n < 11) {
            fields[n++] = token;
            token = strtok(NULL, "|");
        }

        if (n == 11 &&
            atoi(fields[2]) == type &&
            strcmp(fields[3], userId) == 0) {

            printf("\n-----------------------------------------------\n");
            printf("Complaint ID : %s\n", fields[0]);
            printf("Location     : %s\n", fields[4]);
            printf("Category     : %s\n",
                   categoryName((Category)atoi(fields[5])));
            printf("Description  : %s\n", fields[6]);
            printf("Priority     : %s\n",
                   priorityName((Priority)atoi(fields[7])));
            printf("Status       : Resolved\n");
            printf("Assigned To  : %s\n", fields[9]);
            printf("Date         : %s\n", fields[10]);

            found = 1;
        }
    }

    fclose(file);

    if (!found)
        printf("You have no resolved complaints.\n");
}

void viewResolvedHistory(void)
{
    char path[512];
    FILE *file;
    char line[1200];
    int count = 0;

    getResolvedFilePath(path, sizeof(path));

    file = fopen(path, "r");

    printf("\n========== RESOLVED COMPLAINT HISTORY ==========\n");

    if (file == NULL) {
        printf("No resolved complaints yet.\n");
        return;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        char *fields[11];
        char *token;
        int n = 0;

        trimNewline(line);

        token = strtok(line, "|");

        while (token != NULL && n < 11) {
            fields[n++] = token;
            token = strtok(NULL, "|");
        }

        if (n == 11) {
            printf("\n-----------------------------------------------\n");
            printf("Complaint ID : %s\n", fields[0]);
            printf("Name         : %s\n", fields[1]);
            printf("Location     : %s\n", fields[4]);
            printf("Category     : %s\n",
                   categoryName((Category)atoi(fields[5])));
            printf("Description  : %s\n", fields[6]);
            printf("Priority     : %s\n",
                   priorityName((Priority)atoi(fields[7])));
            printf("Status       : Resolved\n");
            printf("Assigned To  : %s\n", fields[9]);
            printf("Date         : %s\n", fields[10]);

            count++;
        }
    }

    fclose(file);

    if (count == 0)
        printf("No resolved complaints yet.");
    else
        printf("\nTotal resolved complaints: %d\n", count);
}

void showStatistics(const ComplaintList *list)
{
    int pending = 0;
    int inProgress = 0;
    int rejected = 0;
    int urgent = 0;
    int resolvedHistory = 0;
    int i;

    for (i = 0; i < list->size; i++) {
        if (list->items[i].status == PENDING)
            pending++;

        if (list->items[i].status == IN_PROGRESS)
            inProgress++;

        if (list->items[i].status == REJECTED)
            rejected++;

        if (list->items[i].priority == URGENT)
            urgent++;
    }

    {
        char path[512];
        FILE *file;
        char line[1200];

        getResolvedFilePath(path, sizeof(path));
        file = fopen(path, "r");

        if (file != NULL) {
            while (fgets(line, sizeof(line), file) != NULL) {
                if (strlen(line) > 1)
                    resolvedHistory++;
            }
            fclose(file);
        }
    }

    printf("\n========== COMPLAINT STATISTICS ==========\n");
    printf("Active Complaints    : %d\n", list->size);
    printf("Pending              : %d\n", pending);
    printf("In Progress          : %d\n", inProgress);
    printf("Rejected             : %d\n", rejected);
    printf("Urgent Active        : %d\n", urgent);
    printf("Resolved History     : %d\n", resolvedHistory);
    printf("-----------------------------------------------\n");
    printf("Total Recorded       : %d\n",
           list->size + resolvedHistory);

    if (list->size > 0) {
        printf("Active Complaint IDs : ");
        displayComplaintIdsRecursively(list, 0);
        printf("\n");
    }
}

void displayComplaintIdsRecursively(const ComplaintList *list, int index)
{
    if (index >= list->size)
        return;

    printf("%d", list->items[index].complaintId);

    if (index < list->size - 1)
        printf(", ");

    displayComplaintIdsRecursively(list, index + 1);
}
