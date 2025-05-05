# KMUTT Book Store Management System

## Overview

The **KMUTT Book Store Management System** is a comprehensive C-based application designed to manage a bookstore's operations. It includes functionalities for both admins and users, such as managing books, orders, coupons, borrowed books, and generating reports. The system uses file-based storage for persistence and provides a user-friendly interface for efficient bookstore management.

## Features

### Admin Features

* **Show Books**: View all books in stock.
* **Show Orders**: View and confirm pending orders.
* **Show History Orders**: View the history of confirmed orders.
* **Show Log Stock Book**: View the log of stock transactions.
* **Show List of Borrowed Books**: View all borrowed books.
* **Show List of Returned Books**: View all returned books.
* **Search Book**: Search for books by category, title, author, or price range.
* **Edit Stock Book**: Edit, delete, add stock, or add new books.
* **Manage Coupons**: Add, edit, delete, or view coupons.
* **Manage Admins**: Add, edit, or delete admin accounts.
* **Generate Report**: Generate a report summarizing total orders, books sold, and revenue.
* **Manage Borrowed Books**: Edit or delete borrowed book records.
* **Exit**: Exit the admin module and return to the login screen.

### User Features

* **View Books**: View all books in stock and add them to the cart.
* **Search Books**: Search for books by category, title, author, or price range.
* **Pre-Order Book**: Pre-order books from incoming stock.
* **Manage Pre-Orders**: View and manage pre-orders.
* **Borrow Book**: Borrow books from the store.
* **Return Book**: Return borrowed books to the store.
* **Process Order**: Process pending orders.
* **Exit**: Exit the user module and return to the login screen.

## File Structure

### Source Files

* `main.c`: Implements user-side functionalities such as viewing books, searching, and managing orders.
* `admin.c`: Implements admin-side functionalities such as managing books, orders, logs, and generating reports.
* `login.c`: Handles user and admin authentication and account creation.
* `search.c`: Implements user-side search functionalities.
* `searchadmin.c`: Implements admin-side search functionalities.

### Data Files

* `file/Book_Stock.csv`: Stores book details (ID, title, author, category, quantity, price).
* `file/Orderlist.csv`: Stores pending orders.
* `file/History_Order.csv`: Stores the history of confirmed orders.
* `file/Log_Stockbook.csv`: Stores stock transaction logs.
* `file/List_borrow_book.csv`: Stores borrowed book records.
* `file/list_return.csv`: Stores returned book records.
* `file/Coupon.csv`: Stores coupon codes and discounts.
* `file/Admin_Password.csv`: Stores admin usernames and passwords.
* `file/Account.csv`: Stores user usernames and passwords.

## How to Run

### 1. Compile the Program or Run on any Code Editor 

Use a C compiler (e.g., GCC) to compile the source files. Run the following commands in the terminal:

```bash
gcc main.c login.c admin.c search.c searchadmin.c -o login.exe
```

Or you can press run on the ```login.c ```page frist.

### 2. Start the Application

Run the executable file:

```bash
./login.exe
```

## Usage

### Login Module

* **Login**: Enter your username and password to log in. Admins are redirected to the admin module, and users to the user module.
* **Create Account**: Create a new user account by entering a unique username and password.
* **Show Books**: View all books in stock without logging in.
* **Exit**: Exit the application.

### Admin Module

Use the admin menu to manage books, orders, logs, coupons, and borrowed books. Generate reports to summarize bookstore performance. Manage admin accounts and access control.

### User Module

Use the user menu to view books, search for books, and manage orders. Borrow and return books as needed. Pre-order books from incoming stock.

## File Formats

1. **Book\_Stock.csv**

   * Stores book details in the format: `ID,Title,Author,Category,Quantity,Price`

2. **Orderlist.csv**

   * Stores pending orders in the format: `OrderID,Username,BookID,Quantity,Date`

3. **History\_Order.csv**

   * Stores confirmed orders in the format: `OrderID,Username,BookID,Quantity,Date,Status`

4. **Log\_Stockbook.csv**

   * Stores stock transaction logs in the format: `TransactionID,BookID,Change,Reason,Date`

5. **List\_borrow\_book.csv**

   * Stores borrowed book records in the format: `BorrowID,Username,BookID,BorrowDate,DueDate`

6. **list\_return.csv**

   * Stores returned book records in the format: `ReturnID,Username,BookID,ReturnDate`

7. **Coupon.csv**

   * Stores coupon codes and discounts in the format: `Code,DiscountPercent,Status`

8. **Admin\_Password.csv**

   * Stores admin credentials in the format: `Username,Password`

9. **Account.csv**

   * Stores user credentials in the format: `Username,Password`

## Error Handling

* **Invalid Input**:

  * The program validates user inputs and displays error messages for invalid entries.
  * Non-numeric input in numeric fields prompts re-entry.

* **File Errors**:

  * If a required file cannot be opened, the program displays an error and exits gracefully.

* **Memory Management**:

  * The program ensures all dynamically allocated memory is freed before exiting.

## Future Improvements

* **Database Integration**:

  * Replace CSV files with a database for scalability and performance.

* **Enhanced Security**:

  * Encrypt passwords for secure storage.

* **Improved User Interface**:

  * Add a graphical user interface (GUI) for better usability.

* **Search Optimization**:

  * Use advanced data structures (e.g., hash tables) for faster search operations.

## Contributors

* **Developer**: Mr. Bannasorn Thongkorn Mr. Supapanya Yathip Mr. Kirakit Kingkaew
* **Institution**: King Mongkut's University of Technology Thonburi (KMUTT)
