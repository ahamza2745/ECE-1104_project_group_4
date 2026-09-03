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
