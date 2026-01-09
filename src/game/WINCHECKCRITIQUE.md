# Win Check Critique
I asked aistudio to critique my wincheck logic and it basically said its inefficient asf. Here is its summary of the issues. Ill fix them another time.

***

Here is a concise summary of the logic issues, broken down by **Efficiency** and **Math Safety**, so you can tackle them yourself.

### 1. Efficiency: The "Copy & Scan" Problem
Your current logic performs two distinct passes for every check:
1.  **Copy Pass:** It iterates through an entire row or diagonal on the board and copies those values into a separate array (`cellCheckBuffer`).
2.  **Scan Pass:** It iterates through that new array to count the connections.

**The issues:**
*   **Double Handling:** You are reading memory, writing it somewhere else, and then reading it again. It is faster to just read the board once and count "on the fly."
*   **Overscoping:** If you have a $100\times100$ board, your logic copies and checks 100 slots for the horizontal check. However, you only need to know if there are `amountToWin` connected pieces touching the *specific slot* that was just played. You don't need to check the far edges of the board.

### 2. Math Safety: The 1D Index Trap
You are using a 1D array (`cells[i]`) and trying to calculate diagonal movement using math like `index - (width + 1)`.

**The issue:**
*   **Integer Underflow:** Your index variables are `size_t`, which are **unsigned** (cannot be negative).
*   If a piece is at index `5` and you try to check the top-left diagonal by subtracting `7` (the width), the result is not `-2`. It wraps around to a massive positive number (like 18 quintillion).
*   This causes your loop to jump to random parts of memory or crash if you aren't extremely careful with your bounds checking.

### Key Concepts for your Rewrite
To improve this, try to design a solution that:
1.  **Stays Local:** Only looks at the neighbors immediately surrounding the last placed piece.
2.  **Uses 2D Coordinates:** Converting your 1D index to `x` and `y` makes checking boundaries (like `x < 0`) much easier than trying to do math on a 1D array index.
3.  **No Buffers:** Counts the matches directly on the `board->cells` array without copying data elsewhere.