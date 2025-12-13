/* main.c - Student Record Management System (SRMS)
 *
 * SRMS that:
 *  - Uses intro.c/intro.h and outro.c/outro.h for welcome/thank-you screens
 *  - Main menu: Student Access | Admin Access | Exit
 *  - All other functionality is inside Admin Menu:
 *      - Add / View / Update / Delete students
 *      - Add / View batches
 *      - Allocation strategies
 *      - Save / Load CSV
 *      - Summary report
 *
 * Compile: gcc -std=c11 -O2 -Wall -o srms main.c intro.c outro.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "intro.h"
#include "outro.h"

#define MAX_STUDENTS 1000
#define MAX_BATCHES 100
#define NAME_LEN 100
#define SAP_LEN 32
#define MAX_LINE_LEN 512

/* ---------------- Data Structures ---------------- */

typedef struct {
    char sap[SAP_LEN];
    char name[NAME_LEN];
    int marks;
    int allocated_batch; /* -1 if none */
} Student;

typedef struct {
    char name[32];
    int capacity;
    int filled;
    int *members; /* indices into students[] */
} Batch;

/* ---------------- Globals ---------------- */

static Student *students = NULL;
static int student_count = 0;

static Batch *batches = NULL;
static int batch_count = 0;

/* ---------------- Utility Prototypes ---------------- */

static void clear_input(void);
static int find_student_by_sap(const char *sap);
static void print_separator(void);
static void safe_strdup_truncate(char *dst, const char *src, size_t n);
static int file_exists(const char *path);

/* ---------------- Student / Batch Prototypes ---------------- */

static void add_student_one(void);
static void add_student_interactive(void);
static void view_students(void);
static void update_student(void);
static void delete_student(void);

static void add_batch(void);
static void view_batches(void);

/* ---------------- Allocation Prototypes ---------------- */

static void allocation_by_marks(void);
static void allocation_alphabetical(int reverse);
static void allocation_by_sap_asc(void);
static void allocation_random(void);

/* ---------------- CSV I/O Prototypes ---------------- */

static void save_csv(const char *filename);
static void load_csv(const char *filename);

/* ---------------- Access & Menus ---------------- */

static void student_access(void);
static void admin_menu(void);

/* ---------------- Reports / Helpers ---------------- */

static void print_summary(void);
static int total_capacity(void);

/* ---------------- Utility Implementations ---------------- */

static void clear_input(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { /* discard */ }
}

static void print_separator(void) {
    printf("-----------------------------------------------------------------\n");
}

static void safe_strdup_truncate(char *dst, const char *src, size_t n) {
    if (n == 0) return;
    strncpy(dst, src ? src : "", n-1);
    dst[n-1] = '\0';
}

static int file_exists(const char *path) {
    if (!path) return 0;
    FILE *f = fopen(path, "r");
    if (f) { fclose(f); return 1; }
    return 0;
}

/* ---------------- Student / Batch Implementations ---------------- */

static int find_student_by_sap(const char *sap) {
    if (!sap) return -1;
    for (int i = 0; i < student_count; ++i) {
        if (strcmp(students[i].sap, sap) == 0) return i;
    }
    return -1;
}

/* Add a single student; no loop here */
static void add_student_one(void) {
    if (student_count >= MAX_STUDENTS) {
        printf("Database is full (max %d). Cannot add more students.\n", MAX_STUDENTS);
        return;
    }

    Student st;
    memset(&st, 0, sizeof st);

    printf("Enter SAP ID: ");
    if (!fgets(st.sap, sizeof st.sap, stdin)) return;
    st.sap[strcspn(st.sap, "\n")] = '\0';
    if (strlen(st.sap) == 0) { printf("SAP ID cannot be empty.\n"); return; }

    if (find_student_by_sap(st.sap) != -1) {
        printf("A student with this SAP already exists.\n");
        return;
    }

    printf("Enter Name: ");
    if (!fgets(st.name, sizeof st.name, stdin)) return;
    st.name[strcspn(st.name, "\n")] = '\0';
    if (strlen(st.name) == 0) { printf("Name cannot be empty.\n"); return; }

    printf("Enter Marks (0-100): ");
    if (scanf("%d", &st.marks) != 1) { clear_input(); printf("Invalid marks input.\n"); return; }
    clear_input();
    if (st.marks < 0 || st.marks > 100) { printf("Marks must be between 0 and 100.\n"); return; }

    st.allocated_batch = -1;

    Student *tmp = realloc(students, sizeof(Student) * (student_count + 1));
    if (!tmp) { printf("Memory allocation failed.\n"); return; }
    students = tmp;
    students[student_count++] = st;
    printf("Student added successfully.\n");
}

/* Interactive wrapper: ask after each addition whether to add another */
static void add_student_interactive(void) {
    for (;;) {
        add_student_one();
        char ans[8];
        printf("Do you want to add another student? (y/n): ");
        if (!fgets(ans, sizeof ans, stdin)) break;
        ans[strcspn(ans, "\n")] = '\0';
        if (strlen(ans) == 0) break;
        char c = tolower(ans[0]);
        if (c == 'y') continue;
        else break;
    }
}

static void view_students(void) {
    if (student_count == 0) {
        printf("No students in the database.\n");
        return;
    }
    print_separator();
    printf("%-10s  %-30s  %-6s  %-6s\n", "SAP", "Name", "Marks", "Batch");
    print_separator();
    for (int i = 0; i < student_count; ++i) {
        int b = students[i].allocated_batch;
        printf("%-10s  %-30s  %-6d  %-6d\n", students[i].sap, students[i].name, students[i].marks, b);
    }
    print_separator();
}

static void update_student(void) {
    char sap[SAP_LEN];
    printf("Enter SAP ID to update: ");
    if (!fgets(sap, sizeof sap, stdin)) return;
    sap[strcspn(sap, "\n")] = '\0';
    int idx = find_student_by_sap(sap);
    if (idx == -1) { printf("Student not found.\n"); return; }

    printf("Current Name: %s\n", students[idx].name);
    printf("Enter new name (or press Enter to keep): ");
    char newname[NAME_LEN];
    if (!fgets(newname, sizeof newname, stdin)) return;
    newname[strcspn(newname, "\n")] = '\0';
    if (strlen(newname) > 0) safe_strdup_truncate(students[idx].name, newname, NAME_LEN);

    printf("Current Marks: %d\nEnter new marks (-1 to keep): ", students[idx].marks);
    int nm;
    if (scanf("%d", &nm) != 1) { clear_input(); printf("Invalid input. Keeping marks.\n"); return; }
    clear_input();
    if (nm >= 0 && nm <= 100) students[idx].marks = nm;
    else if (nm != -1) printf("Marks out of range; keeping old marks.\n");

    printf("Student updated.\n");
}

static void delete_student(void) {
    char sap[SAP_LEN];
    printf("Enter SAP ID to delete: ");
    if (!fgets(sap, sizeof sap, stdin)) return;
    sap[strcspn(sap, "\n")] = '\0';
    int idx = find_student_by_sap(sap);
    if (idx == -1) { printf("Student not found.\n"); return; }

    /* Remove from batches */
    for (int b = 0; b < batch_count; ++b) {
        for (int j = 0; j < batches[b].filled; ++j) {
            if (batches[b].members[j] == idx) {
                for (int k = j; k < batches[b].filled - 1; ++k) batches[b].members[k] = batches[b].members[k + 1];
                batches[b].filled--;
                break;
            }
        }
        for (int j = 0; j < batches[b].filled; ++j) {
            if (batches[b].members[j] > idx) batches[b].members[j]--;
        }
    }

    for (int i = idx; i < student_count - 1; ++i) students[i] = students[i + 1];
    student_count--;
    if (student_count == 0) {
        free(students);
        students = NULL;
    } else {
        Student *tmp = realloc(students, sizeof(Student) * student_count);
        if (tmp) students = tmp;
    }
    printf("Student deleted.\n");
}

/* ---------------- Batch Management ---------------- */

static void add_batch(void) {
    if (batch_count >= MAX_BATCHES) {
        printf("Cannot add more batches (max %d).\n", MAX_BATCHES);
        return;
    }
    Batch b;
    memset(&b, 0, sizeof b);
    printf("Enter batch name: ");
    if (!fgets(b.name, sizeof b.name, stdin)) return;
    b.name[strcspn(b.name, "\n")] = '\0';
    if (strlen(b.name) == 0) { printf("Batch name cannot be empty.\n"); return; }

    printf("Enter batch capacity: ");
    if (scanf("%d", &b.capacity) != 1) { clear_input(); printf("Invalid capacity.\n"); return; }
    clear_input();
    if (b.capacity <= 0) { printf("Capacity must be > 0.\n"); return; }

    b.filled = 0;
    b.members = malloc(sizeof(int) * b.capacity);
    if (!b.members) { printf("Memory error.\n"); return; }

    Batch *tmp = realloc(batches, sizeof(Batch) * (batch_count + 1));
    if (!tmp) { free(b.members); printf("Memory error.\n"); return; }
    batches = tmp;
    batches[batch_count++] = b;
    printf("Batch added.\n");
}

static void view_batches(void) {
    if (batch_count == 0) { printf("No batches defined.\n"); return; }
    for (int i = 0; i < batch_count; ++i) {
        printf("Batch %d: %s (%d/%d)\n", i, batches[i].name, batches[i].filled, batches[i].capacity);
        if (batches[i].filled == 0) { printf("  (no members)\n"); continue; }
        for (int j = 0; j < batches[i].filled; ++j) {
            int si = batches[i].members[j];
            if (si >= 0 && si < student_count)
                printf("   %s - %s (%d)\n", students[si].sap, students[si].name, students[si].marks);
        }
    }
}

/* ---------------- Allocation Helpers ---------------- */

static int cmp_marks_desc(const void *a, const void *b) {
    const Student *sa = a, *sb = b;
    return sb->marks - sa->marks;
}
static int cmp_name_asc(const void *a, const void *b) {
    const Student *sa = a, *sb = b;
    return strcasecmp(sa->name, sb->name);
}
static int cmp_name_desc(const void *a, const void *b) {
    const Student *sa = a, *sb = b;
    return strcasecmp(sb->name, sa->name);
}
static int cmp_sap_asc(const void *a, const void *b) {
    const Student *sa = a, *sb = b;
    return strcasecmp(sa->sap, sb->sap);
}

static void reset_allocations(void) {
    for (int i = 0; i < student_count; ++i) students[i].allocated_batch = -1;
    for (int i = 0; i < batch_count; ++i) batches[i].filled = 0;
}

/* Round-robin allocator respecting capacities */
static void allocate_from_order(const int *order, int order_len) {
    reset_allocations();
    if (order_len == 0) return;
    int cur_batch = 0;
    for (int oi = 0; oi < order_len; ++oi) {
        int sidx = order[oi];
        if (sidx < 0 || sidx >= student_count) continue;
        int placed = 0;
        for (int d = 0; d < batch_count; ++d) {
            int bi = (cur_batch + d) % batch_count;
            if (batches[bi].filled < batches[bi].capacity) {
                batches[bi].members[batches[bi].filled++] = sidx;
                students[sidx].allocated_batch = bi;
                placed = 1;
                cur_batch = (bi + 1) % batch_count;
                break;
            }
        }
        if (!placed) break;
    }
}

/* ---------------- Allocation Strategies ---------------- */

static void allocation_by_marks(void) {
    if (student_count == 0 || batch_count == 0) { printf("Need students and batches to allocate.\n"); return; }
    Student *copy = malloc(sizeof(Student) * student_count);
    if (!copy) { printf("Memory error.\n"); return; }
    for (int i = 0; i < student_count; ++i) copy[i] = students[i];
    qsort(copy, student_count, sizeof(Student), cmp_marks_desc);
    int *order = malloc(sizeof(int) * student_count);
    if (!order) { free(copy); printf("Memory error.\n"); return; }
    for (int i = 0; i < student_count; ++i) order[i] = find_student_by_sap(copy[i].sap);
    allocate_from_order(order, student_count);
    free(order); free(copy);
    printf("Marks-based allocation completed.\n");
}

static void allocation_alphabetical(int reverse) {
    if (student_count == 0 || batch_count == 0) { printf("Need students and batches to allocate.\n"); return; }
    Student *copy = malloc(sizeof(Student) * student_count);
    if (!copy) { printf("Memory error.\n"); return; }
    for (int i = 0; i < student_count; ++i) copy[i] = students[i];
    qsort(copy, student_count, sizeof(Student), reverse ? cmp_name_desc : cmp_name_asc);
    int *order = malloc(sizeof(int) * student_count);
    if (!order) { free(copy); printf("Memory error.\n"); return; }
    for (int i = 0; i < student_count; ++i) order[i] = find_student_by_sap(copy[i].sap);
    allocate_from_order(order, student_count);
    free(order); free(copy);
    printf("Alphabetical allocation %scompleted.\n", reverse ? "reverse " : "");
}

static void allocation_by_sap_asc(void) {
    if (student_count == 0 || batch_count == 0) { printf("Need students and batches to allocate.\n"); return; }
    Student *copy = malloc(sizeof(Student) * student_count);
    if (!copy) { printf("Memory error.\n"); return; }
    for (int i = 0; i < student_count; ++i) copy[i] = students[i];
    qsort(copy, student_count, sizeof(Student), cmp_sap_asc);
    int *order = malloc(sizeof(int) * student_count);
    if (!order) { free(copy); printf("Memory error.\n"); return; }
    for (int i = 0; i < student_count; ++i) order[i] = find_student_by_sap(copy[i].sap);
    allocate_from_order(order, student_count);
    free(order); free(copy);
    printf("SAP ascending allocation completed.\n");
}

static void allocation_random(void) {
    if (student_count == 0 || batch_count == 0) { printf("Need students and batches to allocate.\n"); return; }
    int *idxs = malloc(sizeof(int) * student_count);
    if (!idxs) { printf("Memory error.\n"); return; }
    for (int i = 0; i < student_count; ++i) idxs[i] = i;
    for (int i = student_count - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        int t = idxs[i]; idxs[i] = idxs[j]; idxs[j] = t;
    }
    allocate_from_order(idxs, student_count);
    free(idxs);
    printf("Random allocation completed.\n");
}

/* ---------------- CSV Save/Load ---------------- */

static void save_csv(const char *filename) {
    if (!filename) return;
    FILE *f = fopen(filename, "w");
    if (!f) { printf("Could not open %s for writing.\n", filename); return; }
    fprintf(f, "sap,name,marks,allocated_batch\n");
    for (int i = 0; i < student_count; ++i) {
        char namecopy[NAME_LEN];
        strncpy(namecopy, students[i].name, NAME_LEN-1); namecopy[NAME_LEN-1] = '\0';
        for (char *p = namecopy; *p; ++p) if (*p == ',') *p = ' ';
        fprintf(f, "%s,%s,%d,%d\n", students[i].sap, namecopy, students[i].marks, students[i].allocated_batch);
    }
    fclose(f);
    printf("Saved %d students to %s\n", student_count, filename);
}

static void load_csv(const char *filename) {
    if (!filename) return;
    FILE *f = fopen(filename, "r");
    if (!f) { printf("Could not open %s for reading.\n", filename); return; }

    char line[MAX_LINE_LEN];
    if (!fgets(line, sizeof line, f)) { fclose(f); return; }

    for (int i = 0; i < batch_count; ++i) {
        if (batches[i].members) free(batches[i].members);
    }
    free(batches); batches = NULL; batch_count = 0;

    free(students); students = NULL; student_count = 0;

    while (fgets(line, sizeof line, f)) {
        char *p = strtok(line, ",\n");
        if (!p) continue;
        Student st; memset(&st, 0, sizeof st);
        strncpy(st.sap, p, SAP_LEN-1);
        p = strtok(NULL, ",\n");
        if (p) strncpy(st.name, p, NAME_LEN-1);
        p = strtok(NULL, ",\n");
        if (p) st.marks = atoi(p);
        p = strtok(NULL, ",\n");
        if (p) st.allocated_batch = atoi(p);
        else st.allocated_batch = -1;
        Student *tmp = realloc(students, sizeof(Student) * (student_count + 1));
        if (!tmp) { printf("Memory error while loading.\n"); break; }
        students = tmp;
        students[student_count++] = st;
    }
    fclose(f);
    printf("Loaded %d students from %s\n", student_count, filename);
}

/* ---------------- Student Access ---------------- */

static void student_access(void) {
    char sap[SAP_LEN];
    printf("Enter SAP ID: ");
    if (!fgets(sap, sizeof sap, stdin)) return;
    sap[strcspn(sap, "\n")] = '\0';
    int idx = find_student_by_sap(sap);
    if (idx == -1) { printf("Student not found.\n"); return; }
    printf("SAP: %s\nName: %s\nMarks: %d\n", students[idx].sap, students[idx].name, students[idx].marks);
    int b = students[idx].allocated_batch;
    if (b >= 0 && b < batch_count) printf("Allocated Batch: %s (index %d)\n", batches[b].name, b);
    else printf("Allocated Batch: Not allocated\n");
}

/* ---------------- Admin Menu ---------------- */

static void admin_menu(void) {
    for (;;) {
        printf("\n--- Admin Menu ---\n");
        printf("1. Add student(s)\n2. View students\n3. Update student\n4. Delete student\n");
        printf("5. Add batch\n6. View batches\n7. Allocate batches\n8. Save database to CSV\n");
        printf("9. Load database from CSV\n10. Summary Report\n11. Back to Main Menu\n");
        printf("Choose option: ");
        int ch;
        if (scanf("%d", &ch) != 1) { clear_input(); continue; }
        clear_input();
        if (ch == 1) add_student_interactive();
        else if (ch == 2) view_students();
        else if (ch == 3) update_student();
        else if (ch == 4) delete_student();
        else if (ch == 5) add_batch();
        else if (ch == 6) view_batches();
        else if (ch == 7) {
            if (batch_count == 0) { printf("No batches defined. Add batches first.\n"); continue; }
            if (student_count == 0) { printf("No students available to allocate.\n"); continue; }
            printf("Choose allocation strategy:\n1. Marks (High->Low)\n2. A->Z\n3. Z->A\n4. SAP asc\n5. Random\nSelect: ");
            int s;
            if (scanf("%d", &s) != 1) { clear_input(); continue; }
            clear_input();
            if (s == 1) allocation_by_marks();
            else if (s == 2) allocation_alphabetical(0);
            else if (s == 3) allocation_alphabetical(1);
            else if (s == 4) allocation_by_sap_asc();
            else if (s == 5) allocation_random();
            else printf("Invalid strategy.\n");
        }
        else if (ch == 8) {
            char fname[128];
            printf("Enter filename to save (e.g. students_out.csv): ");
            if (!fgets(fname, sizeof fname, stdin)) continue;
            fname[strcspn(fname, "\n")] = '\0';
            save_csv(fname);
        }
        else if (ch == 9) {
            char fname[128];
            printf("Enter filename to load (e.g. students.csv): ");
            if (!fgets(fname, sizeof fname, stdin)) continue;
            fname[strcspn(fname, "\n")] = '\0';
            load_csv(fname);
        }
        else if (ch == 10) print_summary();
        else if (ch == 11) break;
        else printf("Invalid choice.\n");
    }
}

/* ---------------- Reports / Helpers ---------------- */

static void print_summary(void) {
    printf("\n=== Summary Report ===\n");
    printf("Total students: %d\n", student_count);
    printf("Total batches: %d\n", batch_count);
    int allocated = 0;
    for (int i = 0; i < student_count; ++i) if (students[i].allocated_batch >= 0) allocated++;
    printf("Allocated students: %d\n", allocated);
    printf("Unallocated students: %d\n", student_count - allocated);
    printf("Total capacity: %d\n", total_capacity());
    printf("======================\n");
}

static int total_capacity(void) {
    int sum = 0;
    for (int i = 0; i < batch_count; ++i) sum += batches[i].capacity;
    return sum;
}

/* ---------------- Main ---------------- */

int main(void) {
    srand((unsigned int)time(NULL));

    /* Use intro.c / outro.c screens */
    showIntro();

    /* Auto-load students.csv at startup if present */
    if (file_exists("students.csv")) {
        printf("Detected students.csv in working directory. Loading...\n");
        load_csv("students.csv");
    }

    for (;;) {
        printf("\n=== Main Menu ===\n");
        printf("1. Student Access (search by SAP)\n2. Admin Access\n3. Exit\n");
        printf("Choose option: ");
        int ch;
        if (scanf("%d", &ch) != 1) { clear_input(); continue; }
        clear_input();
        if (ch == 1) student_access();
        else if (ch == 2) admin_menu();
        else if (ch == 3) break;
        else printf("Invalid option.\n");
    }

    /* Cleanup */
    if (students) free(students);
    for (int i = 0; i < batch_count; ++i) {
        if (batches[i].members) free(batches[i].members);
    }
    if (batches) free(batches);

    showThankYou();
    return 0;
}
