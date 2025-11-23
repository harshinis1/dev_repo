def words_on_board(words, board):
    """This function accepts two arguments - words (a list of strings)
    and a Textoggle board, and returns a list of words that can be 
    formed using the letters and blanks available on the board. 
    A blank ('_') is used as a substitute for any letter."""

    # Combine the board into a single list of letters
    board_letters = []
    for row in board:
        board_letters.extend(row)

    # Create a list to store the words that can be sucessfully formed
    valid_words = []

    for word in words:
        # Create a temporary copy of the available letters
        temporary_board = board_letters[:]
        can_form = True
        
        # Check if each character in the word can be found on the board
        for char in word.upper():

            if char in temporary_board:
                # The letters used are replaced with a placeholder
                temporary_board[temporary_board.index(char)] = "#"
            
            elif "_" in temporary_board:
                temporary_board[temporary_board.index("_")] = "#"
            
            else:
                # If the character cannot be matched, the word cannot be formed
                can_form = False
                break

        if can_form:
            valid_words.append(word)

    return valid_words
