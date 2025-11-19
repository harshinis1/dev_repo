def word_on_board(word, board):
    """This function accepts a string and a textoggle board, and
    checks if each letter after the first is an adjacent neighbour 
    (distance of 1) of the preceeding letter. Returns a list 
    of index positions of the letters if the word exists, 
    or none otherwise."""

    # Handles empty strings
    if not word:
        return None
    
    # Create a dictionary to store each letter's position
    # Key is letter and value is (row_index, col_index)
    positions = {}
    for row_index, row in enumerate(board):
        for col_index, letter in enumerate(row):
            positions[letter] = (row_index, col_index)
    
    # Check if the first letter exists for a valid starting point
    if word[0] not in positions:
        return None
    
    # Start the word path with the first letter's position
    path = [positions[word[0]]]

    for i in range(1, len(word)):
        # Check against the last placed letter to make sure word is valid
        current_position = path[-1]
        next_letter = word[i]

        # Check if the next letter exists
        if next_letter not in positions:
            return None
        
        next_position = positions[next_letter]

        # Apply the Euclidean distance formula
        # Define x and y-coordinates separately to abide PEP8 rules
        x = next_position[0]- current_position[0]
        y = next_position[1] - current_position[1]
        distance = ((x)**2 + (y)**2)**0.5
        # Valid distance between adjacent squares must be equal to 1
        if distance != 1: 
            return None
        
        path.append(next_position)  
    
    return path
