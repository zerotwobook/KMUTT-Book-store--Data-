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
    

## Test Cases for KMUTT Book Store Management System


### 1. Login Module (`login.c`)

### Test Case 1: Successful Admin Login
**Input:**  
- Username: `admin1`  
- Password: `password123`  
**Expected Output:**  
- Redirects to the admin module (`admin.exe`)

### Test Case 2: Successful User Login
**Input:**  
- Username: `user1`  
- Password: `userpass`  
**Expected Output:**  
- Redirects to the user module (`main.exe`)

### Test Case 3: Invalid Login
**Input:**  
- Username: `invalidUser`  
- Password: `wrongPass`  
**Expected Output:**  
- Displays: `Invalid username or password.`

### Test Case 4: Create Account with Unique Username
**Input:**  
- Username: `newUser`  
- Password: `newPass123`  
**Expected Output:**  
- Displays: `Account created successfully!`  
- Adds the new account to `Account.csv`

### Test Case 5: Create Account with Existing Username
**Input:**  
- Username: `user1`  
- Password: `newPass123`  
**Expected Output:**  
- Displays: `Username 'user1' already exists. Please choose a different username.`

### Test Case 6: Show Books
**Input:**  
- Select option 3 from the main menu  
**Expected Output:**  
- Displays all books in stock in a tabular format

## 2. User Module (`main.c`)

### Test Case 7: View Books
**Input:**  
- Select option 1 from the user menu  
**Expected Output:**  
- Displays all books in stock with options to add to the cart

### Test Case 8: Search Books by Title
**Input:**  
- Select option 2 from the user menu  
- Enter title: `Harry Potter`  
**Expected Output:**  
- Displays all books with titles starting with "Harry Potter"

### Test Case 9: Borrow a Book
**Input:**  
- Select option 5 from the user menu  
- Enter book title: `The Great Gatsby`  
- Enter quantity: `2`  
**Expected Output:**  
- Displays: `Book borrowed successfully.`  
- Updates the stock in `Book_Stock.csv`

### Test Case 10: Return a Book
**Input:**  
- Select option 6 from the user menu  
- Enter book title: `The Great Gatsby`  
**Expected Output:**  
- Displays: `Book returned successfully.`  
- Updates the stock in `Book_Stock.csv`

### Test Case 11: Pre-Order a Book
**Input:**  
- Select option 3 from the user menu  
- Enter book title: `New Release`  
- Enter quantity: `1`  
**Expected Output:**  
- Displays: `Pre-order placed successfully.`  
- Adds the pre-order to `Orderlist.csv`

## 3. Admin Module (`admin.c`)

### Test Case 12: View Books
**Input:**  
- Select option 1 from the admin menu  
**Expected Output:**  
- Displays all books in stock in a tabular format

### Test Case 13: Edit Book Stock
**Input:**  
- Select option 8 from the admin menu  
- Choose to edit a book  
- Enter book ID: `101`  
- Update quantity: `50`  
**Expected Output:**  
- Displays: `Book details updated successfully.`  
- Updates the stock in `Book_Stock.csv`

### Test Case 14: Add a New Book
**Input:**  
- Select option 8 from the admin menu  
- Choose to add a new book  
- Enter details:  
  - ID: `999`  
  - Title: `New Book`  
  - Author: `Author Name`  
  - Category: `Fiction`  
  - Quantity: `10`  
  - Price: `15.99`  
**Expected Output:**  
- Displays: `New book added successfully.`  
- Adds the new book to `Book_Stock.csv`

### Test Case 15: Generate Report
**Input:**  
- Select option 11 from the admin menu  
**Expected Output:**  
- Displays a report summarizing total orders, books sold, and revenue

### Test Case 16: Manage Coupons
**Input:**  
- Select option 9 from the admin menu  
- Choose to add a coupon  
- Enter coupon code: `DISCOUNT10`  
- Enter discount: `10`  
**Expected Output:**  
- Displays: `Coupon added successfully.`  
- Adds the coupon to `Coupon.csv`

## 4. Search Modules (`search.c` and `searchadmin.c`)

### Test Case 17: Search by Category
**Input:**  
- Select option 1 from the search menu  
- Enter category: `Fiction`  
**Expected Output:**  
- Displays all books in the "Fiction" category

### Test Case 18: Search by Price Range
**Input:**  
- Select option 4 from the search menu  
- Enter minimum price: `10`  
- Enter maximum price: `20`  
**Expected Output:**  
- Displays all books priced between $10 and $20

## 5. Error Handling

### Test Case 19: Invalid Menu Choice
**Input:**  
- Enter an invalid choice (e.g., `5`) in the main menu  
**Expected Output:**  
- Displays: `Invalid choice. Try again.`

### Test Case 20: File Not Found
**Scenario:**  
- Delete `Book_Stock.csv` and attempt to view books  
**Expected Output:**  
- Displays: `Error: Could not open file/Book_Stock.csv.`

### Test Case 21: Invalid Input for Quantity
**Input:**  
- Enter a non-numeric value (e.g., `abc`) for quantity when borrowing a book  
**Expected Output:**  
- Displays: `Invalid input. Please enter a valid number.`

## 6. Integration Tests

### Test Case 22: Admin Workflow
**Steps:**  
- Login as admin  
- View books  
- Edit a book's stock  
- Add a new book  
- Generate a report  
**Expected Output:**  
- All actions are completed successfully, and the corresponding files are updated

### Test Case 23: User Workflow
**Steps:**  
- Login as a user  
- View books  
- Add a book to the cart  
- Checkout  
- Borrow a book  
- Return a book  
**Expected Output:**  
- All actions are completed successfully, and the corresponding files are updated

## Contributors

* **Developer**: Mr. Bannasorn Thongkorn Mr. Supapanya Yathip Mr. Kirakit Kingkaew
* **Institution**: King Mongkut's University of Technology Thonburi (KMUTT)
