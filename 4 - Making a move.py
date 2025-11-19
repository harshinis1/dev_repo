def textoggle_move(board, word_sequence, spare_letters):
    """This function accepts a board, word_sequence and 
    spare letters and returns a new Textoggle board after 
    the word has been identified. The function removes
    identified word letters from the board, shifts remaining
    letters down and fills gaps using spare letters or #. """
    
    # Initialise an empty list to store the copied board
    new_board = []
    for row in board:
        # Slicing for a full copy of each row
        new_board.append(row[:])
    
    rows = len(new_board)
    columns = len(new_board[0])

    # Remove letters in the word sequence by assigning their position to None
    # Using sets to remove duplicate positions
    for row, col in set(word_sequence):
        new_board[row][col] = None
    
    # Iterate through columns to shift remaining letters down
    for col in range(columns):
        column = []
        # Collect all non-empty letters in the current column
        for row in range(rows):
            if new_board[row][col] is not None:
                column.append(new_board[row][col])
        # Place letters from the column back to the board from bottom
        for row in range(rows - 1, -1, -1):
            if column:
                new_board[row][col] = column.pop()
            else:
                new_board[row][col] = None

    # Track usage of spare letters
    spare_index = 0
    # Fill empty spaces with spare letters or #
    for col in range(columns):
        for row in range(rows - 1, -1, -1):
            if new_board[row][col] is None:
                # Check if spare letters are available
                if spare_index < len(spare_letters):
                    new_board[row][col] = spare_letters[spare_index]
                    spare_index += 1
                else:
                    # If no spare letters are left, fill with #
                    new_board[row][col] = "#"

    return new_board
