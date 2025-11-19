points = {"E": 1, "A": 1, "I": 1, "O": 1, "N": 1, "R": 1,
        "T": 1, "L": 1, "S": 1, "U": 1, "D": 2, "G": 2, 
        "B": 3, "C": 3, "M": 3, "P": 3, "F": 4, "H": 4, 
        "V": 4, "W": 4, "Y": 4, "K": 5, "J": 8, "X": 8, 
        "Q": 10, "Z": 10}

def get_legal_words():
    """ This function reads a dictionary file and stores all 
    valid words in a set. """
    
    legal_words = set()
    with open('words_alpha.txt', 'r') as file:
        for line in file:
            # Stores in upper case for consistency
            legal_words.add(line.strip().upper())
    return legal_words


# Using a global variable to reference throughout the program
LEGAL_WORDS = get_legal_words()


def calculate_wordscore(word):
    """ This function calculates the score of a word 
    using the formula given. """

    score = sum(points.get(letter.upper(), 0) for letter in word)
    return score * len(word)

def textoggle_move(board, word_sequence, spare_letters):
    """This function accepts a board, word_sequence and 
    spare letters and returns a new Textoggle board after 
    the word has been identified. The function removes
    identified word letters from the board, shifts remaining
    letters down and fills gaps using spare letters or #. """
    
    # Create a copy of the board to modify the original
    new_board = []
    for row in board:
        new_board.append(row[:])
    rows = len(new_board)
    cols = len(new_board[0])

    for row, col in word_sequence:
        new_board[row][col] = None

    for col in range(cols):
        column = []
        for row in range(rows):
            if new_board[row][col]:
                column.append(new_board[row][col])
        for row in range(rows - 1, -1, -1):
            if column:
                new_board[row][col] = column.pop()
            else:
                new_board[row][col] = None

    spare_index = 0

    for col in range(cols):
        for row in range(rows - 1, -1, -1):
            if new_board[row][col] is None:
                
                if spare_index < len(spare_letters):
                    new_board[row][col] = spare_letters[spare_index]
                    spare_index += 1
                else:
                    new_board[row][col] = "#"

    return new_board


def generate_words_from_position(board, start_row, start_col):
    """ This function iterates over adjacent positions to construct words
    from a starting position and returns a list of words found and 
    their positions. """

    rows, columns = len(board), len(board[0])
    max_word_length = rows * columns
    queue = [(board[start_row][start_col], [(start_row, start_col)])]
    words = []

    while queue:
        current_word, position_sequence = queue.pop(0)
        if len(current_word) > max_word_length:
            continue
        words.append((current_word, position_sequence))
        last_row, last_col = position_sequence[-1]

        for new_row in range(rows):
            for new_col in range(columns):
                # Using the Euclidean distance to check adjacency
                distance = (
                    ((new_row - last_row)**2 + (new_col - last_col)**2)**0.5
                    )
                
                if distance == 1:
                    new_letter = board[new_row][new_col]
                    if new_letter == "_":  
                        for letter in points.keys(): 
                            # Replace blank tiles with all possible letters.
                            new_word = current_word + letter
                            new_position = (new_row, new_col)
                            new_sequence = position_sequence + [(new_position)]
                            queue.append((new_word, new_sequence))
        
                    elif new_letter is not None: 
                        # Continue forming words with adjacent letters
                        new_word = current_word + new_letter
                        new_sequence = position_sequence + [(new_row, new_col)]
                        queue.append((new_word, new_sequence))

    return words


def play_best_game(board, spare_letters):
    """This function plays the best possible game by identifying the
    highest scoring word, updates the board and continues until no 
    valid words remain. Returns the total score achieved in the game. """

    total_score = 0

    while True:
        best_word, best_word_sequence, best_word_score = None, None, 0
        valid_words_found = False
        # Search for words at every position on the board
        for row in range(len(board)):
            for col in range(len(board[0])):
                words = generate_words_from_position(board, row, col)
                for word, word_sequence in words:
                    # Check if the word is valid and has at least 2 letters
                    if len(word) >= 2 and word.lower() in LEGAL_WORDS:
                        valid_words_found = True
                        word_score = calculate_wordscore(word)
                    
                        if word_score > best_word_score or (
                            word_score == best_word_score and word < best_word
                        ):
                            best_word = word
                            best_word_sequence = word_sequence
                            best_word_score = word_score

        if not valid_words_found:
            break

        total_score += best_word_score
        board = textoggle_move(board, best_word_sequence, spare_letters)

    return total_score
