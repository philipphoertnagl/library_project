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

void borrowBookUser(User *user, Book *book) {
    Node *newnodeUser = malloc(sizeof(Node));
    newnodeUser->data = book;
    newnodeUser->rentalTime = time(NULL);
    newnodeUser->due_time = newnodeUser->rentalTime + 24 * 60 * 60;
    newnodeUser->next = NULL;

    //putting chosen book to USER borrowlist
    if (user->borrowedBooks.head == NULL) {
        user->borrowedBooks.head = newnodeUser;
        user->borrowedBooks.temp = newnodeUser;
    } else {
        user->borrowedBooks.temp->next = newnodeUser;
        user->borrowedBooks.temp = newnodeUser;
    }
    user->amountBorrowedBooks++;
}

void checkoutBookLibrary(LinkedList *list, LinkedList *borrowedBooks, BookTitle *title, User *user) {
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
            Node *newnodeBorrowedBooks = malloc(sizeof(Node));
            newnodeBorrowedBooks->data = current->data;
            newnodeBorrowedBooks->rentalTime = time(NULL);
            newnodeBorrowedBooks->due_time = newnodeBorrowedBooks->rentalTime + 24 * 60 * 60;
            newnodeBorrowedBooks->next = NULL;
            // putting chosen book to the borrowlist
            if (borrowedBooks->head == NULL) {
                borrowedBooks->head = newnodeBorrowedBooks;
                borrowedBooks->temp = newnodeBorrowedBooks;
            } else {
                borrowedBooks->temp->next = newnodeBorrowedBooks;
                borrowedBooks->temp = newnodeBorrowedBooks;
            }
            //CREATING NEW BOOK INSTANCES
            Book *newBook = malloc(sizeof(Book));
            memcpy(newBook, current->data, sizeof(Book));    // copy the data from the library's Book instance
            newnodeBorrowedBooks->data = newBook;
            current->data->isBorrowed = true;
            //ATTACH BORROWED BOOK TO USERS BORROWED LIST:
            borrowBookUser(user, newBook);

            char buffer[80];
            due_time_converter(buffer, newnodeBorrowedBooks->due_time);
            printf("1 Book \"%s\" of ID: %d is borrowed by User:\"%s\" , the book has to be returned until: %s\n", title->title,
                   current->data->id, user->name, buffer);
            return;
        }
        current = current->next;
    }
}

char *returnBookUser(LinkedList *userList, int bookID) {
    Node *userNode = userList->head;
    while (userNode != NULL) {
        User *user = (User *) userNode->data;
        LinkedList *userBooks = &user->borrowedBooks;
        Node *bookNode = userBooks->head;
        Node *prevBookNode = NULL;
        while (bookNode != NULL) {
            if (bookNode->data->id == bookID) {
                if (prevBookNode == NULL) {
                    userBooks->head = bookNode->next;
                } else {
                    prevBookNode->next = bookNode->next;
                }
                user->amountBorrowedBooks--;
                free(bookNode);
                return user->name;
            } else {
                prevBookNode = bookNode;
                bookNode = bookNode->next;
            }
        }
        userNode = userNode->next;
    }
    return NULL;
}

void returnBookLibrary(LinkedList *borrowedBooks, LinkedList *userList, int bookID) {
    Node *current = borrowedBooks->head;
    Node *prevnode = NULL;
    char *username;
    while (current != NULL) {
        if (current->data->id == bookID) {
            username = returnBookUser(userList,bookID);
            if(username != NULL) {
                printf("Book %s, example: %d returned by User \" %s \" \n",
                       current->data->bookTitle->title, current->data->id, username);
            }
            else printf("No User found for the book %s, bookID: %d\n", current->data->bookTitle->title, current->data->id);
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
   /* // User list returning
    returnBookUser(userList,bookID);*/
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

void printLibraryInventory(BookTitle *bookTitle) {
        if(bookTitle->totalAmount > 0) {
        printf("Of Title \"%s by %s\" are %d available\n", bookTitle->title, bookTitle->author, bookTitle->totalAmount);
        }
        else {
            printf("Title \"%s by %s\" is not available at the moment\n", bookTitle->title, bookTitle->author);
        }
}

void printList(LinkedList *list) {
    Node *current = list->head;
    while (current != NULL) {
        printf("Book ID: %d of Book: %s\n", current->data->id, current->data->bookTitle->title);
        current = current->next;
    }
}

void printUser(LinkedList *userlist, User *user) {
    Node *current = user->borrowedBooks.head;
    printf("\nUser: %s has %d books borrowed. ", user->name, user->amountBorrowedBooks);
    if (user->amountBorrowedBooks > 0) {
        printf("The books are: ");
    }
    while(current != NULL) {
        if (user->amountBorrowedBooks > 0) {
            if(current->next != NULL) {
                printf("%s, ", current->data->bookTitle->title);
            }
             else printf("%s", current->data->bookTitle->title);
            }
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
    BookTitle *bookTitle1, *bookTitle2, *bookTitle3, *bookTitle4;
    bookTitle1 = createBookTitle("Max Frisch", "Biedermann & Brandstifter", 2);
    bookTitle2 = createBookTitle("Leo Tolstoy", "Krieg & Frieden", 5);
    bookTitle3 = createBookTitle("Fjodor Dostoevskiy", "Idiot", 15);
    bookTitle4 = createBookTitle("Franz Kafka", "Die Verwandlung", 42);

    //CREATING LISTS
    LinkedList linkedList;
    initializeList(&linkedList);
    addBookCopiesToLibrary(&linkedList, bookTitle1);
    addBookCopiesToLibrary(&linkedList, bookTitle2);
    addBookCopiesToLibrary(&linkedList, bookTitle3);
    addBookCopiesToLibrary(&linkedList, bookTitle4);
    printList(&linkedList);

    //DEFINING USERS
    User *user1, *user2, *user3, *user4;
    user1 = createUser("Johnny");
    user2 = createUser("Sepp");
    user3 = createUser("Philippovic");
    user4 = createUser("Dinara");

    LinkedList userList;
    initializeList(&userList);
    addUserToList(&userList, user1);
    addUserToList(&userList, user2);
    addUserToList(&userList, user3);
    addUserToList(&userList, user4);

printf("\nLibrary Inventory:\n");
    printLibraryInventory(bookTitle1);
    printLibraryInventory(bookTitle2);
    printLibraryInventory(bookTitle3);
    printLibraryInventory(bookTitle4);

    //BORROWING
    LinkedList borrowedBooks;
    initializeList(&borrowedBooks);

    printf("\n\n Borrow Station: \n\n");
    checkoutBookLibrary(&linkedList, &borrowedBooks, bookTitle1, user1);
    checkoutBookLibrary(&linkedList, &borrowedBooks, bookTitle2, user2);
    checkoutBookLibrary(&linkedList, &borrowedBooks, bookTitle2, user3);
    checkoutBookLibrary(&linkedList, &borrowedBooks, bookTitle4, user2);
    checkoutBookLibrary(&linkedList, &borrowedBooks, bookTitle3, user3);
    checkoutBookLibrary(&linkedList, &borrowedBooks, bookTitle1, user4);
    checkoutBookLibrary(&linkedList, &borrowedBooks, bookTitle3, user4);
    checkoutBookLibrary(&linkedList, &borrowedBooks, bookTitle4, user1);
    checkoutBookLibrary(&linkedList, &borrowedBooks, bookTitle4, user3);
    checkoutBookLibrary(&linkedList, &borrowedBooks, bookTitle4, user4);


    printf("\nLibrary Inventory:\n");
    printLibraryInventory(bookTitle1);
    printLibraryInventory(bookTitle2);
    printLibraryInventory(bookTitle3);
    printLibraryInventory(bookTitle4);

    //ALL BOOKS PRINTED
    printf("\n\nCurrently these books are all borrowed from someone in the library: \n\n");
    printList(&borrowedBooks);



    printUser(&borrowedBooks,user1);
    printUser(&borrowedBooks,user2);
    printUser(&borrowedBooks,user3);


    //RETURNING
    printf("\n\nRETURN Box:\n");
    returnBookLibrary(&borrowedBooks, &userList, 2);
    returnBookLibrary(&borrowedBooks, &userList, 22);


    //PRINT USERS
    printUser(&borrowedBooks,user1);
    printUser(&borrowedBooks,user2);
    printUser(&borrowedBooks,user3);


printf("\n");
    return 0;
}



// code in .h und .c funktionen splitten (GITHUB!!)
//datenspeicherung
//dann binary trees? oder zuerst next step im learning plan