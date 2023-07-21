/*
Track users: Create a User struct that keeps track of a user's name and the books they have checked out. You could then create functions to add a user, delete a user, print all users, etc.

Save and load data: Add functions to save the current state of the library (all books, their availability, all users and their books) to a file and to load this data from a file.
 This way, you won't lose all of your data every time you run the program.

Book categories: Add a category field to the Book struct and create functions to print all books in a certain category, check out a book from a certain category, etc.

Error checking and handling: Right now, your program assumes that everything will go according to plan (e.g., malloc will always succeed, books will always be available to check out, etc.). You could a
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

typedef struct {
    char title[50];
    char author[50];
    int totalAmount;
    int isAvailable;
}BookTitle;

typedef struct {
    BookTitle *bookTitle;
    int id;
    bool isBorrowed;
} Book;

typedef struct Node{
    Book *data;
    time_t due_time;
    time_t rentalTime;
    struct Node *next;
}Node;

typedef struct LinkedList{
    Node *head;
    Node *temp;
} LinkedList;

typedef struct {
    char name[25];
    int amountBorrowedBooks; //50 books borrowed is maximum per user
    LinkedList borrowedBooks;
}User;

User *createUser(const char *username) {
    User *newUser = malloc(sizeof(User));
    strcpy(newUser->name, username);
    newUser->amountBorrowedBooks = 0;
    return newUser;
}

BookTitle *createBookTitle(const char *author_name, const char *title_name, int numberCopies) {
    BookTitle *newTitle = malloc(sizeof(BookTitle));
    newTitle->isAvailable = newTitle->totalAmount ? 1 : 0;
    newTitle->totalAmount = numberCopies;
    strcpy(newTitle->author, author_name);
    strcpy(newTitle->title, title_name);

    return newTitle;
}

Book *createBook(BookTitle *titleOfBook) {
    Book *newBook = malloc(sizeof(Book));
    static int current_id = 0;
    newBook->bookTitle = titleOfBook;
    newBook->id = current_id++;
    newBook->isBorrowed = false;

    return newBook;
}

void due_time_converter(char* buffer, time_t due_time) {
    struct tm *timeinfo;
    timeinfo = localtime(&due_time);
    strftime(buffer, 80, "Due date: %Y-%m-%d %H:%M:%S", timeinfo);
}

void checkoutBook(LinkedList *list, LinkedList *borrowedBooks, BookTitle *title, User *user) {
    if (title->totalAmount < 0) {
        printf("No books available of this title, please try later\n");
        return;
    }
    Node *current = list->head;
    while (current != NULL) {
        if (current->data->bookTitle == title && current->data->isBorrowed == false) {
            current->data->isBorrowed = true;
            title->totalAmount--;
            title->isAvailable = title->totalAmount ? 1 : 0;

            //picking book
            Node *newnode = malloc(sizeof(Node));
            newnode->data = current->data;
            newnode->rentalTime = time(NULL);
            newnode->due_time = newnode->rentalTime + 2 * 7 * 24 * 60 * 60;
            newnode->next = NULL;

            // putting chosen book to the borrowlist
            if (borrowedBooks->head == NULL) {
                borrowedBooks->head = newnode;
                borrowedBooks->temp = newnode;
            } else {
                borrowedBooks->temp->next = newnode;
                borrowedBooks->temp = newnode;
            }

            //putting chosen book to USER borrowlist
            if (user->borrowedBooks.head == NULL) {
                user->borrowedBooks.head = newnode;
                user->borrowedBooks.temp = newnode;
            } else {
                user->borrowedBooks.temp->next = newnode;
                user->borrowedBooks.temp = newnode;
            }
            user->amountBorrowedBooks++;

            char buffer[80];
            due_time_converter(buffer, newnode->due_time);
            printf("1 Book \"%s\" of ID: %d is borrowed, the book has to be returned until: %s\n", title->title,
                   current->data->id, buffer);
            return;
        }
        current = current->next;
    }
}


void returnBook(LinkedList *borrowedBooks, LinkedList *userList, int bookID) {
    Node *current = borrowedBooks->head;
    Node *prevnode = NULL;
    
    while (current != NULL) {
        if (current->data->id == bookID) {
            printf("Book %s, example: %d returned\n", current->data->bookTitle->title, current->data->id);
            if (prevnode == NULL) {
                borrowedBooks->head = current->next;
            } else {
                prevnode->next = current->next;
            }

            current->data->bookTitle->totalAmount++;
            current->data->bookTitle->isAvailable = 1;
            current->data->isBorrowed = false;
            current->due_time = 0;
            free(current);
            break;
        }
        else {
            prevnode = current;
            current = current->next;
        }
    }
    User *currentUser = userList->head;
    while(currentUser != NULL) {

    }
}

void initializeList(LinkedList *list) {
    list->head = NULL;
    list->temp = NULL;
}

void addUserToList(LinkedList *userlist, User *user) {
    Node *newnode = malloc(sizeof(Node));
    newnode->data = user;
    newnode->next = NULL;
    if(userlist->head == NULL) {
        userlist->head = newnode;
        userlist->temp = newnode;
    }
    else {
        userlist->temp->next = newnode;
        userlist->temp = newnode;
    }
}


void addToList(LinkedList *list, Book *book) {
    Node *newnode = malloc(sizeof(Node)); //creates a new node of the size struct Node times the exemplaries of the book
    newnode->data = book;
    newnode->next = NULL;
    if(list->head == NULL) {
        list->head = newnode;
        list->temp = newnode;
    }
    else {
        list->temp->next = newnode;
        list->temp = newnode;
    }
}

void addBookCopiesToLibrary(LinkedList *list, BookTitle *bookTitle) {
    for(int i = 0; i < bookTitle->totalAmount; i++) {
        Book *newBook = createBook(bookTitle);
        addToList(list, newBook);
    }
}


void printList(LinkedList *list) {
    Node *current = list->head;
    while (current != NULL) {
        printf("Example: %d of Book: %s\n", current->data->id, current->data->bookTitle->title);
        current = current->next;
    }
}

void printUser(LinkedList *userlist, User *user) {
    Node *current = user->borrowedBooks.head;
    printf("\nUser: %s has %d books borrowed. The books are: ", user->name, user->amountBorrowedBooks);
    while(current != NULL) {
        printf("%s, ", current->data->bookTitle->title);
        current = current->next;
    }
}

void printOverdueBooks(LinkedList *list) {
    Node *current = list->head;
    while(current != NULL) {
            if(current->due_time - time(NULL) < 0) {
                printf("The book title %s ID: %d is overdue", current->data->bookTitle->title, current->data->id);

            }
            current = current->next;
    }
}

int main() {
    //DEFINING TITLES & AMOUNT OF BOOKS HELD IN LIBRARY
    BookTitle *bookTitle1, *bookTitle2;
    bookTitle1 = createBookTitle("Max Frisch", "Biedermann und die Brandstifter", 2);
    bookTitle2 = createBookTitle("Ryan Gosling", "Harry Potter", 50);

    //CREATING LISTS
    LinkedList linkedList;
    initializeList(&linkedList);
    addBookCopiesToLibrary(&linkedList, bookTitle1);
    addBookCopiesToLibrary(&linkedList, bookTitle2);
    printList(&linkedList);

    //DEFINING USERS
    User *user1, *user2, *user3;
    user1 = createUser("Johnny");
    user2 = createUser("Sepp");
    user3 = createUser("Philippovic");
    //BORROWING
    LinkedList borrowedBooks;
    initializeList(&borrowedBooks);
    checkoutBook(&linkedList, &borrowedBooks, bookTitle1,user1);
    checkoutBook(&linkedList, &borrowedBooks, bookTitle2, user2);
    checkoutBook(&linkedList, &borrowedBooks, bookTitle2, user3);
    checkoutBook(&linkedList, &borrowedBooks, bookTitle2, user2);
    checkoutBook(&linkedList, &borrowedBooks, bookTitle2, user3);
    checkoutBook(&linkedList, &borrowedBooks, bookTitle2, user2);
    printList(&borrowedBooks);
    //RETURNING
    returnBook(&borrowedBooks, 4);
    returnBook(&borrowedBooks, 6);
    printList(&borrowedBooks);

    printUser(&borrowedBooks,user1);

    printUser(&borrowedBooks,user2);

    printUser(&borrowedBooks,user3);

    return 0;
}

// addUserList() machen, in returnBooks() nach user und dann aus borrowedList von User löschen
// checkoutBook funktion in 2-3 teile splitten
//vlt weiter noch
//dann binary trees? oder zuerst next step im learning plan