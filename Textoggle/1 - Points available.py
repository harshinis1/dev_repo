def calculate_value(board):
    """ Accepts letters on a board as input and returns the total
    points value of all letters on the board using Scrabble's point
    system. """

    # Create a dictionary containing each letter (key) and its score (value)
    points = {"E": 1, "A": 1, "I": 1, "O": 1, "N": 1, "R": 1,
        "T": 1, "L": 1, "S": 1, "U": 1, "D": 2, "G": 2,
        "B": 3, "C": 3, "M": 3, "P": 3, "F": 4, "H": 4, 
        "V": 4, "W": 4, "Y": 4, "K": 5, "J": 8, "X": 8, 
        "Q": 10, "Z": 10}
    
    total_score = 0

    for row in board:
        for square in row:
            # Add each letter's point to the total value
            total_score += points.get(square.upper(), 0)
    
    return total_score
