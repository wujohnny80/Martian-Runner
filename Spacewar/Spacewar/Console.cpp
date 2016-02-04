#include "Console.h"

//=============================================================================
// Constructor
//=============================================================================
Console::Console()
{
    initialized = false;                // set true when successfully initialized
    graphics = nullptr;
    visible = false;                    // not visible
    fontColor = consoleNS::FONT_COLOR;
    backColor = consoleNS::BACK_COLOR;

    x = consoleNS::X;                   // starting console position
    y = consoleNS::Y;

    textRect.bottom = consoleNS::Y + consoleNS::HEIGHT - consoleNS::MARGIN;
    textRect.left = consoleNS::X + consoleNS::MARGIN;
    textRect.right = consoleNS::X + consoleNS::WIDTH - consoleNS::MARGIN;
    textRect.top = consoleNS::Y + consoleNS::MARGIN;

    vertexBuffer = NULL;

    rows = 0;
    scrollAmount = 0;
}

//=============================================================================
// Destructor
//=============================================================================
Console::~Console()
{
    onLostDevice();            // call onLostDevice() for every graphics item
}

//=============================================================================
// Initialize the console
//=============================================================================
bool Console::initialize(Graphics *g, Input *in)
{
    try 
	{
        graphics = g;                    // the graphics system
        input = in;

		// fill the vertex data
		// x,y are the screen location
		// z is the depth on the screen
		// rhw is always set to 1.0f
		// the color is the color at that vertex, and colors the quad

        // top left
        vtx[0].x = x;
        vtx[0].y = y;
        vtx[0].z = 0.0f;
        vtx[0].rhw = 1.0f;
        vtx[0].color = backColor;

        // top right
        vtx[1].x = x + consoleNS::WIDTH;
        vtx[1].y = y;
        vtx[1].z = 0.0f;
        vtx[1].rhw = 1.0f;
        vtx[1].color = backColor;

        // bottom right
        vtx[2].x = x + consoleNS::WIDTH;
        vtx[2].y = y + consoleNS::HEIGHT;
        vtx[2].z = 0.0f;
        vtx[2].rhw = 1.0f;
        vtx[2].color = backColor;

        // bottom left
        vtx[3].x = x;
        vtx[3].y = y + consoleNS::HEIGHT;
        vtx[3].z = 0.0f;
        vtx[3].rhw = 1.0f;
        vtx[3].color = backColor;

		// the order is important!
		// we add to the buffer in clockwise order, if this order is not followed, the quad will not be visible
		// DirectX draws only one side of the triangle, because by default DirectX uses back face culling. only the front side is drawn
		// if we add the vertices in counterclockwise order, we will be viewing the back, and thus, nothing.

        graphics->CreateVertexBuffer(vtx, sizeof vtx, vertexBuffer);

        // initialize DirectX font
        if(dxFont.initialize(graphics, consoleNS::FONT_HEIGHT, false,
                             false, consoleNS::FONT) == false)
            return false;      // if failed
        dxFont.setFontColor(fontColor);

    } 
	catch(...) 
	{
        return false;
    }

    initialized = true;
    return true;
}

//=============================================================================
// draw console
// Pre: Inside BeginScene/EndScene
//=============================================================================
const void Console::draw()
{
    if (!visible || graphics == NULL || !initialized)
        return;

    graphics->DrawQuad(vertexBuffer);       // draw backdrop
    if(text.size() == 0)
        return;

    graphics->SpriteBegin();                // Begin drawing sprites

    // display text on console
    textRect.left = 0;
    textRect.top = 0;

    // sets textRect bottom to height of 1 row
    dxFont.print("|",textRect,DT_CALCRECT);     
    int rowHeight = textRect.bottom + 2;    // height of 1 row (+2 is row spacing)
    if(rowHeight <= 0)                      // this should never be true
        rowHeight = 20;                     // force a workable result

    // number of rows that will fit on console
    rows = (consoleNS::HEIGHT - 2*consoleNS::MARGIN) / rowHeight;
    rows -= 2;                              // room for input prompt at bottom
    if (rows <= 0)                          // this should never be true
        rows = 5;                           // force a workable result

    // set text display rect for one row
    textRect.left = (long)(x + consoleNS::MARGIN);
    textRect.right = (long)(textRect.right + consoleNS::WIDTH - consoleNS::MARGIN);
    // -2*rowHeight is room for input prompt
    textRect.bottom = (long)(y + consoleNS::HEIGHT - 2*consoleNS::MARGIN - 2*rowHeight);
    // for all rows (max text.size()) from bottom to top
    for(int r=scrollAmount; r<rows+scrollAmount && r<(int)(text.size()); r++)
    {
        // set text display rect top for this row
        textRect.top = textRect.bottom - rowHeight; 
        // display one row of text
        dxFont.print(text[r],textRect,DT_LEFT);     
        // adjust text display rect bottom for next row
        textRect.bottom -= rowHeight;               
    }

    // display command prompt and current command string
    // set text display rect for prompt
    textRect.bottom = (long)(y + consoleNS::HEIGHT - consoleNS::MARGIN);
    textRect.top = textRect.bottom - rowHeight;
    std::string prompt = ">";                   // build prompt string
    prompt += input->GetTextIn();
    dxFont.print(prompt,textRect,DT_LEFT);      // display prompt and command

    graphics->SpriteEnd();                      // End drawing sprites
}

//=============================================================================
// show/hide console
//=============================================================================
void Console::showHide() 
{
    if (!initialized)
        return;
    visible = !visible;
    input->Clear(InputNS::KEYS_PRESSED|InputNS::TEXT_IN);    // erase old input
}            


//=============================================================================
// Add text to console
// Only the first line of text in str will be displayed.
//=============================================================================
void Console::print(const std::string &str)     // add text to console
{
    if (!initialized)
        return;
    text.push_front(str);                       // add str to deque of text
    if(text.size() > consoleNS::MAX_LINES)
        text.pop_back();                        // delete oldest line
}

//=============================================================================
// Return console command
// Handles console single key commands.
// Returns all other commands to game.
//=============================================================================
std::string Console::getCommand()
{
    // if console not initialized or not visible
    if (!initialized || !visible)               
        return "";

    //check for console key
    if (input->WasKeyPressed(Key::TILDE))
        hide();                                 // turn off console

    //check for Esc key
	if (input->WasKeyPressed(Key::ESC))
        return "";

    // check for scroll
	if (input->WasKeyPressed(Key::UP_ARROW))            // if up arrow
        scrollAmount++;
    else if (input->WasKeyPressed(Key::DOWN_ARROW))     // if down arrow
        scrollAmount--;
	else if (input->WasKeyPressed(Key::PGUP))    // if page up
        scrollAmount += rows;
	else if (input->WasKeyPressed(Key::PGDN))     // if page down
        scrollAmount -= rows;
    if (scrollAmount < 0)
        scrollAmount = 0;
    if (scrollAmount > consoleNS::MAX_LINES-1)
        scrollAmount = consoleNS::MAX_LINES-1;
    if (scrollAmount > (int)(text.size())-1)
        scrollAmount = (int)(text.size())-1;

    commandStr = input->GetTextIn();            // get user entered text
    // do not pass keys through to game
    input->Clear(InputNS::KEYS_DOWN|InputNS::KEYS_PRESSED|InputNS::MOUSE);

    if (commandStr.length() == 0)               // if no command entered
        return "";
    if (commandStr.at(commandStr.length()-1) != '\r')   // if 'Enter' key not pressed
        return "";                              // return, can't be command

    commandStr.erase(commandStr.length()-1);    // erase '\r' from end of command string
    input->ClearTextIn();                       // clear input line
    inputStr = commandStr;                      // save input text
    return commandStr;                          // return command
}


//=============================================================================
// called when graphics device is lost
//=============================================================================
void Console::onLostDevice()
{
    if (!initialized)
        return;
    dxFont.onLostDevice();
    SAFE_RELEASE(vertexBuffer);
}

//=============================================================================
// called when graphics device is reset
//=============================================================================
void Console::onResetDevice()
{
    if (!initialized)
        return;
    graphics->CreateVertexBuffer(vtx, sizeof vtx, vertexBuffer);
    dxFont.onResetDevice();
}

