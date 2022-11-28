#include "Board.h"
#include <queue>
#include <cmath>
#include <sstream>
#include <iomanip>

using std::queue;

/*==== Private Functions ====*/

Board::Tile::Tile()
{
    /*
    Tile constructor.  Sets the fill color, size, outline color and thickness.
    */
    tile.setFillColor(sf::Color::Black);
    tile.setSize(sf::Vector2f(xSize, ySize));
    tile.setOutlineColor(sf::Color::Red);
    tile.setOutlineThickness(borderThickness);
    isSource = false;
    isDest = false;
    isObst = false;
}

void Board::Tile::setTileColor(sf::Color c)
{
    /*
    Sets the fill color for the tile.
    */
    tile.setFillColor(c);
}

void Board::makeGraphs()
{
    /*
    Constructs both graph implementations: map and linked list (LL).
    */
    int indexNum = 0; // Index of each tile. 0 to (tilesX - 1) for tiles in row 0, tilesX to 2*tilesX - 1 for row 1, etc.

    // At {i, j} grid position, contruct a tile and a node, set its position.  Make node at {0, 0} the LL head node.
    for (int i = 0; i < tilesY; i++)
    {
        for (int j = 0; j < tilesX; j++)
        {
            Tile* tile = new Tile();
            tile->index = indexNum++;
            Node* node = new Node(tile);
            posToTile[std::make_pair(i, j)] = std::make_pair(tile, node);
            tile->tile.setPosition(sf::Vector2f(borderThickness + j * (xSize + 2 * borderThickness), borderThickness + i * (ySize + 2 * borderThickness)));
            graphMap[tile] = {};

            if (i == 0 && j == 0)
                head = node;
        }
    }

    // For each tile/node, create the edges to nearest neighbors for the map, and the nearest neighbor pointers for the LL.
    for (int i = 0; i < tilesY; i++)
    {
        for (int j = 0; j < tilesX; j++)
        {
            insertEdges(i, j);
            setLLPointers(i, j);
        }
    }
}

void Board::insertEdges(int i, int j)
{
    /*
    Add nearest neighbor edges for the tile at position {i, j}.
    */
    if (i == 0 && j == 0) // Top left corner tile has 3 nearest neighbors.
    {
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i + 1, j}].first); // 1, 0
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i, j + 1}].first); // 0, 1
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i + 1, j + 1}].first); // 1, 1
    }

    else if (i == 0 && j == tilesX - 1) // Top right corner tile has 3 nearest neighbors.
    {
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i, j - 1}].first); // 0, 248
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i + 1, j}].first); // 1, 249
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i + 1, j - 1}].first); // 1, 248
    }

    else if (i == tilesY - 1 && j == tilesX - 1) // Bottom right corner tile has 3 nearest neighbors.
    {
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i - 1, j}].first); // 248, 249
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i - 1, j - 1}].first); // 248, 248
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i, j - 1}].first); // 249, 248
    }

    else if (i == tilesY - 1 && j == 0) // Bottom left corner tile has 3 nearest neighbors.
    {
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i - 1, j}].first); // 248, 0
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i - 1, j + 1}].first); // 248, 1
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i, j + 1}].first); // 249, 1
    }
    
    else if (i == 0 && j > 0 && j < tilesX - 1) // Top border tile has 5 nearest neighbors.
    {
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i, j + 1}].first);
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i, j - 1}].first);
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i + 1, j - 1}].first);
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i + 1, j}].first);
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i + 1, j + 1}].first);
    }

    else if (i > 0 && i < tilesY - 1 && j == tilesX - 1) // Right border tile has 5 nearest neighbors.
    {
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i - 1, j}].first);
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i - 1, j - 1}].first);
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i, j - 1}].first);
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i + 1, j - 1}].first);
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i + 1, j}].first);
    }

    else if (i == tilesY - 1 && j > 0 && j < tilesX - 1) // Bottom border tile has 5 nearest neighbors.
    {
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i, j - 1}].first);
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i, j + 1}].first);
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i - 1, j - 1}].first);
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i - 1, j}].first);
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i - 1, j + 1}].first);
    }

    else if (i > 0 && i < tilesY - 1 && j == 0) // Left border tile has 5 nearest neighbors.
    {
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i - 1, j}].first);
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i - 1, j + 1}].first);
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i, j + 1}].first);
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i + 1, j + 1}].first);
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i + 1, j}].first);
    }
    
    else if (i > 0 && i < tilesY - 1 && j > 0 && j < tilesX - 1) // Interior tile has 8 nearest neighbors.
    {
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i - 1, j - 1}].first);
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i - 1, j}].first);
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i - 1, j + 1}].first);
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i, j - 1}].first);
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i, j + 1}].first);
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i + 1, j - 1}].first);
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i + 1, j}].first);
        graphMap[posToTile[{i, j}].first].insert(posToTile[{i + 1, j + 1}].first);
    }
}

void Board::setLLPointers(int i, int j)
{
    /*
    Add pointers to nearest neighbors for the node at position {i, j}.
    */
    if (i == 0 && j == 0) // Top left corner.
    {
        posToTile[{i, j}].second->down = posToTile[{i + 1, j}].second; // 1, 0
        posToTile[{i, j}].second->right = posToTile[{i, j + 1}].second; // 0, 1
        posToTile[{i, j}].second->botRight = posToTile[{i + 1, j + 1}].second; // 1, 1
    }

    else if (i == 0 && j == tilesX - 1) // Top right corner.
    {
        posToTile[{i, j}].second->left = posToTile[{i, j - 1}].second; // 0, 248
        posToTile[{i, j}].second->down = posToTile[{i + 1, j}].second; // 1, 249
        posToTile[{i, j}].second->botLeft = posToTile[{i + 1, j - 1}].second; // 1, 248
    }

    else if (i == tilesY - 1 && j == tilesX - 1) // Bottom right corner.
    {
        posToTile[{i, j}].second->up = posToTile[{i - 1, j}].second; // 248, 249
        posToTile[{i, j}].second->topLeft = posToTile[{i - 1, j - 1}].second; // 248, 248
        posToTile[{i, j}].second->left = posToTile[{i, j - 1}].second; // 249, 248
    }

    else if (i == tilesY - 1 && j == 0) // Bottom left corner.
    {
        posToTile[{i, j}].second->up = posToTile[{i - 1, j}].second; // 248, 0
        posToTile[{i, j}].second->topRight = posToTile[{i - 1, j + 1}].second; // 248, 1
        posToTile[{i, j}].second->right = posToTile[{i, j + 1}].second; // 249, 1
    }
    
    else if (i == 0 && j > 0 && j < tilesX - 1) // Top border.
    {
        posToTile[{i, j}].second->right = posToTile[{i, j + 1}].second;
        posToTile[{i, j}].second->left = posToTile[{i, j - 1}].second;
        posToTile[{i, j}].second->botLeft = posToTile[{i + 1, j - 1}].second;
        posToTile[{i, j}].second->down = posToTile[{i + 1, j}].second;
        posToTile[{i, j}].second->botRight = posToTile[{i + 1, j + 1}].second;
    }

    else if (i > 0 && i < tilesY - 1 && j == tilesX - 1) // Right border.
    {
        posToTile[{i, j}].second->up = posToTile[{i - 1, j}].second;
        posToTile[{i, j}].second->topLeft = posToTile[{i - 1, j - 1}].second;
        posToTile[{i, j}].second->left = posToTile[{i, j - 1}].second;
        posToTile[{i, j}].second->botLeft = posToTile[{i + 1, j - 1}].second;
        posToTile[{i, j}].second->down = posToTile[{i + 1, j}].second;
    }

    else if (i == tilesY - 1 && j > 0 && j < tilesX - 1) // Bottom border.
    {
        posToTile[{i, j}].second->left = posToTile[{i, j - 1}].second;
        posToTile[{i, j}].second->right = posToTile[{i, j + 1}].second;
        posToTile[{i, j}].second->topLeft = posToTile[{i - 1, j - 1}].second;
        posToTile[{i, j}].second->up = posToTile[{i - 1, j}].second;
        posToTile[{i, j}].second->topRight = posToTile[{i - 1, j + 1}].second;
    }

    else if (i > 0 && i < tilesY - 1 && j == 0) // Left border.
    {
        posToTile[{i, j}].second->up = posToTile[{i - 1, j}].second;
        posToTile[{i, j}].second->topRight = posToTile[{i - 1, j + 1}].second;
        posToTile[{i, j}].second->right = posToTile[{i, j + 1}].second;
        posToTile[{i, j}].second->botRight = posToTile[{i + 1, j + 1}].second;
        posToTile[{i, j}].second->down = posToTile[{i + 1, j}].second;
    }
    
    else if (i > 0 && i < tilesY - 1 && j > 0 && j < tilesX - 1) // Interior.
    {
        posToTile[{i, j}].second->topLeft = posToTile[{i - 1, j - 1}].second;
        posToTile[{i, j}].second->up = posToTile[{i - 1, j}].second;
        posToTile[{i, j}].second->topRight = posToTile[{i - 1, j + 1}].second;
        posToTile[{i, j}].second->left = posToTile[{i, j - 1}].second;
        posToTile[{i, j}].second->right = posToTile[{i, j + 1}].second;
        posToTile[{i, j}].second->botLeft = posToTile[{i + 1, j - 1}].second;
        posToTile[{i, j}].second->down = posToTile[{i + 1, j}].second;
        posToTile[{i, j}].second->botRight = posToTile[{i + 1, j + 1}].second;
    }
}

void Board::displayBoard(sf::RenderWindow& window)
{
    /*
    Display the grid of tiles using the window.draw() SFML function.
    */
    map<Tile*, set<Tile*>>::iterator iter = graphMap.begin();
	for (; iter != graphMap.end(); ++iter)
        window.draw(iter->first->tile);
    
    // Display the text showing instructions and results in the right margin of the window.
    displayText(window);
}

void Board::displayText(sf::RenderWindow& window)
{
    /*
    Displays the text and buttons in window.  Variables text1 - text10 are displayed depending on state of the board.
    */

    // Font of display text.  Load font from file.
    sf::Font font;
    font.loadFromFile("fonts/Roboto-Light.ttf");

    if (source == nullptr)  // User needs to select a starting tile.
    {
        text1.setFont(font);
        text1.setString("Select start tile");
        text1.setPosition(sf::Vector2f(1715.f, 0.f));
        text1.setCharacterSize(20);
        text1.setFillColor(sf::Color::White);
        window.draw(text1);
    }
    
    else if (source != nullptr && destin == nullptr) // Starting tile selected.  Now user must select an end tile.
    {
        text1.setFont(font);
        text1.setString("Select end tile");
        text1.setPosition(sf::Vector2f(1715.f, 0.f));
        text1.setCharacterSize(20);
        text1.setFillColor(sf::Color::White);
        window.draw(text1);
    }

    // This text always shows.  Instructs user to right-click mouse on a tile to select it as an obstacle.
    text2.setFont(font);
    text2.setString("RC for Obstacles");
    text2.setPosition(sf::Vector2f(1715.f, 25.f));
    text2.setCharacterSize(20);
    text2.setFillColor(sf::Color::White);
    window.draw(text2);

    // This text always shows.  Instructs user to select a graph implementation type.
    text3.setFont(font);
    text3.setString("Select graph type");
    text3.setPosition(sf::Vector2f(1715.f, 60.f));
    text3.setCharacterSize(20);
    text3.setFillColor(sf::Color::White);
    window.draw(text3);

    // Displays the LL button.
    linkedListTexture.loadFromFile("images/linkedlist.png");
    linkedListSprite.setTexture(linkedListTexture);
    linkedListSprite.setPosition(sf::Vector2f(1765.f, 90.f));
    window.draw(linkedListSprite);
    
    // Displays the map button.
    mapTexture.loadFromFile("images/map.png");
    mapSprite.setTexture(mapTexture);
    mapSprite.setPosition(sf::Vector2f(1845.f, 90.f));
    window.draw(mapSprite);
    
    // Indicates whether user selected LL or map.
    text6.setFont(font);
    text6.setPosition(sf::Vector2f(1715.f, 145.f));
    text6.setCharacterSize(20);
    text6.setFillColor(sf::Color::White);
    
    if (linkedListSelected)
    {
        text6.setString("Linked list selected");
        window.draw(text6);
    }

    else if (mapSelected)
    {
        text6.setString("Map selected");
        window.draw(text6);
    }

    // Instructs user to click reset button to reset the board.
    text4.setFont(font);
    text4.setString("Click to reset");
    text4.setPosition(sf::Vector2f(1715.f, 165.f));
    text4.setCharacterSize(20);
    text4.setFillColor(sf::Color::White);
    window.draw(text4);

    // Displays the reset button.
    resetTexture.loadFromFile("images/reset.png");
    resetSprite.setTexture(resetTexture);
    resetSprite.setPosition(sf::Vector2f(1765.f, 195.f));
    window.draw(resetSprite);

    // Once user selects a starting and ending tile, and a graph type, this text will appear, instructing
    // user to click the Go button to find the shortest path.
    if (source != nullptr && destin != nullptr && (mapSelected || linkedListSelected))
    {
        text5.setFont(font);
        text5.setString("Go for shortest path");
        text5.setPosition(sf::Vector2f(1715.f, 255.f));
        text5.setCharacterSize(20);
        text5.setFillColor(sf::Color::White);
        window.draw(text5);
        
        // Displays the Go button.
        goTexture.loadFromFile("images/go.jpg");
        goSprite.setTexture(goTexture);
        goSprite.setPosition(sf::Vector2f(1765.f, 285.f));
        window.draw(goSprite);
    }

    // Text variables text7 to text10 only displays after the shortest path algorithm finishes running.
    // When the algorithm finishes, a shortestPath vector gets populated and becomes nonempty.

    // If shortestPath vector is nonempty and first element is nullptr, no path can be found.
    if (shortestPath.size() > 0 && shortestPath[0] == nullptr)
    {
        text7.setFont(font);
        text7.setString("No path exists!");
        text7.setPosition(sf::Vector2f(1715.f, 340.f));
        text7.setCharacterSize(20);
        text7.setFillColor(sf::Color::White);
        window.draw(text7);
    }

    // If shortestPath vector is nonempty and first element is not nullptr, then a path was found.
    else if (shortestPath.size() > 0 && shortestPath[0] != nullptr)
    {
        text7.setFont(font);
        string message1 = "Shortest path is:";
        text7.setString(message1);
        text7.setPosition(sf::Vector2f(1715.f, 340.f));
        text7.setCharacterSize(20);
        text7.setFillColor(sf::Color::White);
        window.draw(text7);

        // This text displays the number of moves from source tile to destination tile in the shortest path found.
        text8.setFont(font);
        string message2 = std::to_string(shortestPath.size());

        if (shortestPath.size() == 1)
            message2 += " move!";

        else
            message2 += " moves!";

        text8.setString(message2);
        text8.setPosition(sf::Vector2f(1715.f, 365.f));
        text8.setCharacterSize(20);
        text8.setFillColor(sf::Color::White);
        window.draw(text8);
    }

    // This text displays the time taken for the algorithm to run.
    if (shortestPath.size() > 0)
    {
        text9.setFont(font);
        string message1 = "Time taken is:";
        text9.setString(message1);
        text9.setPosition(sf::Vector2f(1715.f, 390.f));
        text9.setCharacterSize(20);
        text9.setFillColor(sf::Color::White);
        window.draw(text9);

        // Calculate time taken, round to 2 decimal places, and display the result.
        double timeTaken = duration.count();
        std::stringstream stream;
        stream << std::fixed << std::setprecision(2) << timeTaken;
        std::string s = stream.str();

        text10.setFont(font);
        string message2 = stream.str() + " ms";
        text10.setString(message2);
        text10.setPosition(sf::Vector2f(1715.f, 415.f));
        text10.setCharacterSize(20);
        text10.setFillColor(sf::Color::White);
        window.draw(text10);
    }
}

int* Board::shortestPathGraph()
{
    /*
    Finds shortest path for the map implementation.
    */

    // Start the clock.
    auto start = high_resolution_clock::now();

    // Total number of vertices, including the obstacles (for simplicity at the cost of extra memory used).
    int numVertices = graphMap.size();
    // Queue containing vertices that need to be visited.
    queue<int> q;
    // Create new boolean array of size numVertices saying whether tile at each index has been visited by algorithm.
    bool* visited = new bool[numVertices] {false};
    // endFound is true if ending tile was visited, false otherwise.
    bool endFound = false;

    // Create new int array of size numVertices, saying which tile is the parent/predecessor of the tile at given index.
    // Example: If p[3] = 5, then vertex 5 comes before vertex 3 in the shortest path.
    int* p = new int[numVertices] {-1};
    
    // Indices of source and destination tiles.
    int src = source->index;
    int dest = destin->index;
    
    // Source tile is visited first.
    visited[src] = true;

    q.push(src);

    // While the queue is nonempty and the ending tile has not been visited yet, visit the next tile in the queue,
    // mark each of its nearest neighbors as visited if they haven't been visited yet and are not obstacles, and 
    // add each one to the queue if they are not obstacles.  If ending tile is found, empty the queue and break out
    // of the for-loop.
    while (!q.empty() && !endFound)
    {
        int u = q.front();
        visited[u] = true;
        q.pop();

        int i = u / tilesX;
        int j = u % tilesX;
        Tile* current = posToTile[{i, j}].first;
        set<Tile*> adj = graphMap[current];

        for (Tile* n: adj)
        {
            int v = n->index;
			
            if (!visited[v] && !(n->isObst))
            {
                visited[v] = true;
				p[v] = u;
				
				if (v == destin->index)
				{
					endFound = true;
					break;
				}
				
                else
                    q.push(v);
            }
        }
    }

    // If ending tile wasn't found, set p[ending tiles's index] = -1.
    if (!endFound)
        p[destin->index] = -1;

    // Algorithm over, stop the clock.
    auto stop = high_resolution_clock::now();
    duration = stop - start;
    
    // Free up memory taken up by visited.
    delete[] visited;
    visited = nullptr;

    return p;
}

Board::Node* Board::traverseLL(int index)
{
    /*
    Traverses the LL from head node to node at given index and returns a pointer to that node.
    We are traversing the list as if it was a line.  We start at the head and go right until we reach the end of
    row 0, then down to row 1 and go to the left until we reach the left end of row 1, go down, and go to the right,
    and so on, until we reach the node.
    */
    Node* temp = head;
    // Node at index is at position {indexI, indexJ} on the grid.
    int indexI = index / tilesX;
    int indexJ = index % tilesX;

    // If the node is at an odd-numbered row, we traverse that row in the leftward direction.
    // This results in a larger or smaller number of total moves from head to the node.
    // Example: if node is at indexI = 1 and indexJ = 0, its "effective index" would not be tilesX but
    // tilesX + (tilesX - 1).  If tilesX was 10, then instead of index being 10, it would be 19 and a total
    // of 19 moves from head to node would be required.  This redefinition of index would not be necessary if
    // node was located at an even-numbered row. 
    if (indexI % 2 == 1)
        index = (indexI * tilesX) + (tilesX - 1 - indexJ);

    // Starting from the head, traverse the list index number of moves to reach the desired node.
    for (int u = 0; u < index; u++)
    {
        int i = u / tilesX;
        int j;

        // If current node (temp) is on an odd-numbered row, redefine current column number so that we move
        // in the correct direction.  Do the same for if the row is even-numbered.
        if (i % 2 == 1)
            j = tilesX - (u % tilesX) - 1;

        else
            j = u % tilesX;

        // Depending on current node's position, move to the right, left, or down.
        if (i % 2 == 0 && j < tilesX - 1)
            temp = temp->right;

        else if (i % 2 == 0 && j == tilesX - 1)
            temp = temp->down;

        else if (i % 2 == 1 && j > 0)
            temp = temp->left;

        else if (i % 2 == 1 && j == 0)
            temp = temp->down;
    }

    return temp;
}

int* Board::shortestPathLL()
{
    /*
    Traverse the linked list from head to source and return the source node.
    Then traverse the linked list from head to destin and return the destin node.
    Then find the shortest path between the two nodes.
    There are many ways to optimize this, but the point is to use a linked list as a linked list
    whereby to get to a given node, one must traverse the entire list as if it were a line.
    */
    int u = source->index;
    int v = destin->index;
    
    // Start the clock.
    auto start = high_resolution_clock::now();
    Node* src = traverseLL(u);
    Node* dest = traverseLL(v);

    // Find shortest path between source (start) and destination (end) tiles.
    int* p = shortestPathNodes(src, dest);
    // Algorithm over, stop the clock.
    auto stop = high_resolution_clock::now();
    duration = stop - start;

    return p;
}

int* Board::shortestPathNodes(Node* start, Node* end)
{
    /*
    Does the same as shortestPathGraph, but for the LL implementation.  The same comments apply as in that function,
    but we will comment code that applies specifically to this function.
    */
    int numVertices = graphMap.size();
    queue<int> q;
    bool* visited = new bool[numVertices] {false};
    bool endFound = false;
    int* p = new int[numVertices] {-1};
    int strt = start->tPtr->index;
    int dest = end->tPtr->index;
    visited[strt] = true;

    q.push(strt);

    while (!q.empty() && !endFound)
    {
        int u = q.front();
        visited[u] = true;
        
        q.pop();

        int i = u / tilesX;
        int j = u % tilesX;
        Node* current = posToTile[{i, j}].second;
        // Set of nodes consisting of nearest neighbor nodes of current node in the queue.
        set<Node*> adj;

        // Insert each nearest neighbor into adj.
        adj.insert(current->botLeft);
        adj.insert(current->down);
        adj.insert(current->botRight);
        adj.insert(current->left);
        adj.insert(current->right);
        adj.insert(current->topLeft);
        adj.insert(current->up);
        adj.insert(current->topRight);

        for (Node* n: adj)
        {
            // If one of the nearest neighbors is nonexistent, move onto the next nearest neighbor node.
            // For example, if current node is at row 0, there will be no nearest neighbors above the node.
            if (n == nullptr)
                continue;

            int v = n->tPtr->index;
			
            if (!visited[v] && !(n->tPtr->isObst))
            {
                visited[v] = true;
				p[v] = u;
				
				if (v == dest)
				{
					endFound = true;
					break;
				}
				
                else
                    q.push(v);
            }
        }
    }

    if (!endFound)
        p[dest] = -1;

    delete[] visited;
    visited = nullptr;

    return p;
}

void Board::displayShortestPath(int* p)
{
    /*
    This function will take the p array and push the tiles making up the shortest path to vector shortestPath.
    Each tile (except source and destination tiles) will have its color changed to yellow.  If no path exists,
    push nullptr to shortestPath and exit the function.
    */
    if (p[destin->index] == -1)
        shortestPath.push_back(nullptr);

    else
    {
        shortestPath.push_back(source);
        int prev = p[destin->index];
	
	    while (prev != source->index)
	    {
		    int i = prev / tilesX;
            int j = prev % tilesX;
            posToTile[{i, j}].first->setTileColor(sf::Color::Yellow);
            shortestPath.push_back(posToTile[{i, j}].first);
		    prev = p[prev];
	    }
    }

    delete[] p;
    p = nullptr;
}

void Board::resetBoard()
{
    /*
    This function runs when user selects to reset board button.  Board is reset so that all tiles are black and
    all selections are undone.  It allows the user to start over.
    */

    // Reset source tile selection.
    if (source != nullptr)
    {
        source->setTileColor(sf::Color::Black);
        source->isSource = false;
        source = nullptr;
    }

    // Reset destination tile selection.
    if (destin != nullptr)
    {
        destin->setTileColor(sf::Color::Black);
        destin->isDest = false;
        destin = nullptr;
    }

    // Reset member variables to their default values.
    mapSelected = false;
    linkedListSelected = false;
    goButtonClicked = false;
    source = nullptr;
    destin = nullptr;

    // If any obstacles were selected, undo the selections by setting the relevant variables to false,
    // and reset the color of the obstacle tiles to black.
    for (auto obstacle: obstacles)
    {
        obstacle->setTileColor(sf::Color::Black);
        obstacle->isObst = false;
    }

    // Clear the obstacles set.
    obstacles.clear();

    // If the shortest path algorithm was previously run, shortestPath vector will be populated
    // If vector has tiles making up a previously found shortest path, reset their color to black
    if (shortestPath.size() > 0 && shortestPath[0] != nullptr)
    {
        for (int i = 0; i < shortestPath.size(); i++)
            shortestPath[i]->setTileColor(sf::Color::Black);
    }

    // Clear the shortestPath vector.
    shortestPath.clear();
}

/*==== Public Functions ====*/

Board::Board()
{
    /*
    Board constructor.  Sets member variables to default values.
    */
    head = nullptr;
    source = nullptr;
    destin = nullptr;
    mapSelected = false;
    linkedListSelected = false;
    goButtonClicked = false;
    makeGraphs();
}

Board::~Board()
{
    /*
    Destructor that will free up any remaining memory, clearing out the data structures.
    */
    map<pair<int, int>, pair<Tile*, Node*>>::iterator iter;
    vector<Node*> nodesToDelete;
    vector<Tile*> tilesToDelete;

    // Iterate through m and add each node and tile pointer to the associated vector.
    for (iter = posToTile.begin(); iter != posToTile.end(); ++iter)
    {
        nodesToDelete.push_back(iter->second.second);
        tilesToDelete.push_back(iter->second.first);
    }

    // Delete each Node and Tile, and set each pointer to nullptr.
    for (int i = 0; i < nodesToDelete.size(); i++)
    {
        delete nodesToDelete[i];
        delete tilesToDelete[i];
        nodesToDelete[i] = nullptr;
        tilesToDelete[i] = nullptr;
    }

    // Clear member vectors/maps and graph data structures.  Set pointers to nullptr.
    posToTile.clear();
    graphMap.clear();
    shortestPath.clear();
    obstacles.clear();
    head = nullptr;
    source = nullptr;
    destin = nullptr;
}

void Board::play(sf::RenderWindow& window)
{
    /*
    This function is the function that runs in main() and is responsible for displaying the window, board, text,
    and is responsible for accepting user input and outputting results.
    */
    while (window.isOpen())
    {
        // Clear contents of previous frame.
        window.clear();

        // Event object: events include mouse being pressed, keyboard press, etc.
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Event recorded is one that is reponsible for closing the window.
            if (event.type == sf::Event::Closed)
                window.close();

            // User pressed a mouse button.
            else if (event.type == sf::Event::MouseButtonPressed)
            {
                // Record the position of the mouse when user pressed a mouse button.
                sf::Vector2i position = sf::Mouse::getPosition(window);
                // Find what tile the user clicked on (if any).
                int i = position.y / (ySize + 1);
                int j = position.x / (xSize + 1);

                // User left-clicked on a tile.
                if (event.mouseButton.button == sf::Mouse::Left && i < tilesY && j < tilesX)
                {
                    // Source tile has not been selected yet, so tile selected will be source tile,
                    // as long as it wasn't selected to be an obstacle.
                    if (source == nullptr && !posToTile[{i, j}].first->isObst)
                    {
                        posToTile[{i, j}].first->setTileColor(sf::Color::Green);
                        source = posToTile[{i, j}].first;
                        posToTile[{i, j}].first->isSource = true;
                    }

                    // Source tile was already selected and user clicked on that same tile: undo selection.
                    else if (source == posToTile[{i, j}].first && destin == nullptr)
                    {
                        posToTile[{i, j}].first->setTileColor(sf::Color::Black);
                        source = nullptr;
                        posToTile[{i, j}].first->isSource = false;
                    }

                    // Source tile was already selected and user clicked on a different tile, setting it as the destination,
                    // as long as it wasn't selected to be an obstacle.
                    else if (source != posToTile[{i, j}].first && destin == nullptr && !posToTile[{i, j}].first->isObst)
                    {
                        posToTile[{i, j}].first->setTileColor(sf::Color::Red);
                        destin = posToTile[{i, j}].first;
                        posToTile[{i, j}].first->isDest = true;
                    }

                    // Destination tile was already selected and user clicked on that same tile: undo selection.
                    else if (destin == posToTile[{i, j}].first)
                    {
                        posToTile[{i, j}].first->setTileColor(sf::Color::Black);
                        destin = nullptr;
                        posToTile[{i, j}].first->isDest = false;
                    }
                }

                // User right-clicked on a tile and the Go button wasn't pressed.
                else if (event.mouseButton.button == sf::Mouse::Right && i < tilesX && j < tilesY && !goButtonClicked)
                {
                    // User right-clicked on an empty tile (not source and not destination)
                    // Set that tile as an obstacle tile.
                    if (source != posToTile[{i, j}].first && destin != posToTile[{i, j}].first)
                    {
                        posToTile[{i, j}].first->setTileColor(sf::Color::Magenta);
                        posToTile[{i, j}].first->isObst = true;
                        obstacles.insert(posToTile[{i, j}].first);
                    }

                    // This doesn't work for some reason.  Purpose is to undo selection of an obstacle tile.
                    //else if (obstacles.count(posToTile[{i, j}].first))
                    else if (posToTile[{i, j}].first->isObst)
                    {
                        posToTile[{i, j}].first->setTileColor(sf::Color::Black);
                        posToTile[{i, j}].first->isObst = false;
                        obstacles.erase(posToTile[{i, j}].first);
                    }
                }

                // User left-clicked off the grid, where user-selections take place.
                else if (event.mouseButton.button == sf::Mouse::Left && j > tilesY)
                {
                    // User clicked on the LL button before Go button was pressed, thereby choosing the LL implementation.
                    if(linkedListSprite.getGlobalBounds().contains(position.x, position.y) && !linkedListSelected && !goButtonClicked)
                    {
                        linkedListSelected = true;
                        mapSelected = false;
                    }

                    // User clicked on the map button before Go button was pressed, thereby choosing the map implementation.
                    else if (mapSprite.getGlobalBounds().contains(position.x, position.y) && !mapSelected && !goButtonClicked)
                    {
                        mapSelected = true;
                        linkedListSelected = false;
                    }

                    // User clicked on reset board button.
                    else if (resetSprite.getGlobalBounds().contains(position.x, position.y))
                        resetBoard();

                    // User clicked on Go button, so run the appropriate shortest path algorithm, depending on which graph implemented they selected.
                    else if (goSprite.getGlobalBounds().contains(position.x, position.y) && shortestPath.size() == 0)
                    {
                        goButtonClicked = true;
                        int* p;

                        // User selected the map implementation.
                        if (mapSelected)
                            p = shortestPathGraph();

                        // User selected the LL implementation.
                        else if (linkedListSelected)
                            p = shortestPathLL();

                        // After algorithm finishes, display the shortest path if it exists.
                        displayShortestPath(p);
                    }
                }
            }
        }

        // After all updates have finished processing, display the updated state of the board.
        displayBoard(window);
        window.display();
    }
}