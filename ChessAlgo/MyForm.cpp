#include "MyForm.h"
#include "InputBox.h"

namespace ChessAlgo {
    const int BOARD_SIZE = 8;
    const int ETUDES_NUMBER = 4;
    const int TASK_NUMBER = 8;

    static struct Params {
        bool whiteTurn;
        string taskText;
        int taskNumber = 1;
    } params;

    // �������� ������������ ����� �� ��������� �����
    char board[BOARD_SIZE][BOARD_SIZE];

    // ������� ��� �������� ������ �� �����
    void MyForm::loadTaskFromFile(string filename, int taskNumber, bool& whiteTurn, string& text) {
        ifstream file(filename);
        if (!file.is_open()) {
            labelText->Text = "������ �������� �����\n";
            return;
        }
        setlocale(LC_ALL, "rus");

        // ��������� ������ �� ������ ���
        for (int i = 1; i < taskNumber; ++i) {
            string line;
            getline(file, line); // ����� ������
            getline(file, line); // ��� ���
            // ��������� �����
            for (int row = 0; row < BOARD_SIZE; ++row) {
                getline(file, line);
                memcpy(board[row], line.c_str(), BOARD_SIZE);
            }
            getline(file, line); // ������ ������ ����� ��������
        }

        // ��������� ������ ������
        getline(file, text); // ����� ������
        string turnString;
        getline(file, turnString); // ��� ���
        whiteTurn = (turnString == "��� �����");
        // ��������� �����
        for (int row = 0; row < BOARD_SIZE; ++row) {
            string line;
            getline(file, line);
            memcpy(board[row], line.c_str(), BOARD_SIZE);
        }

        // ��������� ����
        file.close();
    }

    // ������� ��� ������ ������������ ����� �� ����� �� �����
    void MyForm::printBoard() {
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                buttons[i, j]->Text = Char::ToString(board[i][j]);
            }
        }
    }

    bool MyForm::isInBoard(int x, int y) {
        return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
    }

    bool MyForm::isPathClear(int fromX, int fromY, int toX, int toY) {
        int deltaX = toX - fromX;
        int deltaY = toY - fromY;
        int stepX = deltaX == 0 ? 0 : deltaX / abs(deltaX);
        int stepY = deltaY == 0 ? 0 : deltaY / abs(deltaY);

        for (int i = fromX + stepX, j = fromY + stepY; i != toX || j != toY; i += stepX, j += stepY) {
            if (board[j][i] != '.') {
                return false;
            }
        }
        return true;
    }

    // ������� ��� �����������, ����� �� ������ ������� �� ��������� �������
    bool MyForm::canMove(int fromX, int fromY, int toX, int toY, bool whiteTurn) {
        if (!isInBoard(fromX, fromY) || !isInBoard(toX, toY) || (fromX == toX && fromY == toY)) {
            return false;
        }

        char piece = board[fromY][fromX];
        char target = board[toY][toX];

        if (piece == '.') {
            return false;
        }

        bool is_white = isupper(piece);
        bool target_is_white = isupper(target);

        if (is_white != whiteTurn) {
            return false;
        }

        if (is_white == target_is_white && target != '.') {
            return false;
        }

        int deltaX = abs(toX - fromX);
        int deltaY = abs(toY - fromY);
        int directionY = is_white ? -1 : 1;

        switch (tolower(piece)) {
        case 'p': // �����
            if (deltaX == 0) {
                if (deltaY == 1) {
                    return (toY == fromY + directionY) && (target == '.');
                }
                else if (deltaY == 2) {
                    if (!is_white && fromY == 1) {
                        return target == '.' && (fromY + 2 == toY) && isPathClear(fromX, fromY, toX, toY);
                    }
                    else if (is_white && fromY == 6) {
                        return target == '.' && (fromY - 2 == toY) && isPathClear(fromX, fromY, toX, toY);
                    }
                }
            }
            else if (deltaX == 1 && deltaY == 1) {
                return (toY == fromY + directionY && target != '.');
            }
            return false;

        case 'r': // �����
            if (deltaX == 0 || deltaY == 0) {
                return isPathClear(fromX, fromY, toX, toY);
            }
            return false;

        case 'n': // ����
            return (deltaX == 1 && deltaY == 2) || (deltaX == 2 && deltaY == 1);

        case 'b': // ����
            if (deltaX == deltaY) {
                return isPathClear(fromX, fromY, toX, toY);
            }
            return false;

        case 'q': // �����
            if ((deltaX == 0) != (deltaY == 0) || deltaX == deltaY) {
                return isPathClear(fromX, fromY, toX, toY);
            }
            return false;

        case 'k': // ������
            return (deltaX <= 1 && deltaY <= 1);

        default:
            return false;
        }
    }

    // ������� ��� ����������� ������ �� �������� �������
    void MyForm::move(int fromX, int fromY, int toX, int toY) {
        board[toY][toX] = board[fromY][fromX];
        board[fromY][fromX] = '.';
    }

    // ������� ��� �������� ����
    bool MyForm::isCheck(bool whiteKing) {
        int kingX = -1;
        int kingY = -1;

        // ���� ������� ������
        for (int y = 0; y < BOARD_SIZE; ++y) {
            for (int x = 0; x < BOARD_SIZE; ++x) {
                char piece = board[y][x];
                if (tolower(piece) == 'k' && isupper(piece) == whiteKing) {
                    kingX = x;
                    kingY = y;
                    break;
                }
            }
            if (kingX != -1) {
                break;
            }
        }

        if (kingX == -1 || kingY == -1) {
            // ��� ������ �� �����
            return false;
        }

        // ��������, ����� �� ���-�� �� ���������� ��������� �� ������
        for (int y = 0; y < BOARD_SIZE; ++y) {
            for (int x = 0; x < BOARD_SIZE; ++x) {
                char piece = board[y][x];
                if (piece != '.' && isupper(piece) != whiteKing) {
                    if (canMove(x, y, kingX, kingY, !whiteKing)) {
                        return true;
                    }
                }
            }
        }

        return false;
    }

    vector<pair<int, int>> MyForm::possibleMoves(int x, int y) {
        vector<pair<int, int>> moves;

        if (board[y][x] == '.') return moves;

        for (int newY = 0; newY < BOARD_SIZE; ++newY) {
            for (int newX = 0; newX < BOARD_SIZE; ++newX) {
                if (canMove(x, y, newX, newY, isupper(board[y][x])))
                    moves.emplace_back(newX, newY);
            }
        }
        return moves;
    }

    // ������� ��� �������� ���a
    bool MyForm::isCheckmate(bool whiteKing) {
        if (!isCheck(whiteKing)) {
            return false;
        }

        for (int y = 0; y < BOARD_SIZE; ++y) {
            for (int x = 0; x < BOARD_SIZE; ++x) {
                char piece = board[y][x];
                if (piece != '.' && isupper(piece) == whiteKing) {
                    vector<pair<int, int>> moves = possibleMoves(x, y);
                    for (pair<int, int> new_coords : moves) {
                        int newX = new_coords.first;
                        int newY = new_coords.second;

                        // ��������� ������� ��������� �����
                        char tempPiece = board[newY][newX];

                        // ���������� ���
                        board[y][x] = '.';
                        board[newY][newX] = piece;

                        if (!isCheck(whiteKing)) {
                            // �������� ���
                            board[y][x] = piece;
                            board[newY][newX] = tempPiece;
                            return false;
                        }

                        // �������� ���
                        board[y][x] = piece;
                        board[newY][newX] = tempPiece;
                    }
                }
            }
        }

        return true;
    }

    void MyForm::startGame()
    {
        for (int i = 0; i < BOARD_SIZE; i++)
        {
            for (int j = 0; j < BOARD_SIZE; j++)
            {
                buttons[i, j]->Enabled = true;
            }
        }

        isSelected = false;
        totalMoves = 0;

        labelInfo->Text = "����� �����: " + totalMoves + "\n~~��� " + (params.whiteTurn ? "�����" : "������") + "~~\n";
        printBoard();

        groupBox1->Visible = true;
    }

    System::Void MyForm::button1_Click(System::Object^ sender, System::EventArgs^ e) {
        ChessAlgo::InputBox inputBox(
            "��� � 1 ���",
            "��� ������� ����� ��� ����������� ��������� ��� ������ ��������� �� 1 ���.\n������� ����� ������ �� 1 �� " + TASK_NUMBER + ": ",
            &params.taskNumber
            );
        inputBox.ShowDialog();

        while (params.taskNumber < 1 || params.taskNumber > TASK_NUMBER) {
            inputBox.changeText(
                "��� � 1 ���", 
                "������� ������ ����� ������. ���������� �����.\n������� ����� ������ �� 1 �� " + TASK_NUMBER + ": ");
            inputBox.ShowDialog();
        }

        loadTaskFromFile("mate1step.txt", params.taskNumber, params.whiteTurn, params.taskText);
        labelInfo->Text = msclr::interop::marshal_as<System::String^>(params.taskText);

        for (int i = 0; i < BOARD_SIZE; i++)
        {
            for (int j = 0; j < BOARD_SIZE; j++)
            {
                buttons[i, j]->Enabled = true;
            }
        }

        isSelected = false;

        printBoard();

        groupBox1->Visible = true;

        isFirstTask = true;
        isStandartTask = false;
        isSecondTask = false;
    }

    System::Void MyForm::button2_Click(System::Object^ sender, System::EventArgs^ e) {
        ChessAlgo::InputBox inputBox(
            "����",
            "� ����� �� ������ ����������� ��������� ������� ������ ��� ��� �������� �������� �� ���������� ����� �����\n������� ����� ����� �� 1 �� " + ETUDES_NUMBER + ": ",
            &params.taskNumber
        );
        inputBox.ShowDialog();

        while (params.taskNumber < 1 || params.taskNumber > TASK_NUMBER) {
            inputBox.changeText(
                "����",
                "������� ������ ����� �����. ���������� �����.\n������� ����� ����� �� 1 �� " + ETUDES_NUMBER + ": ");
            inputBox.ShowDialog();
        }

        loadTaskFromFile("etude.txt", params.taskNumber, params.whiteTurn, params.taskText);

        startGame();

        isStandartTask = true;
        isSecondTask = true;
        isFirstTask = false;
    }

    System::Void MyForm::button3_Click(System::Object^ sender, System::EventArgs^ e) {
        loadTaskFromFile("board.txt", 1, params.whiteTurn, params.taskText);

        startGame();

        isFirstTask = false;
        isSecondTask = false;
        isStandartTask = true;
    }

    System::Void MyForm::button4_Click(System::Object^ sender, System::EventArgs^ e) {
        MessageBox::Show(
            "����� ���������� � �������!\n����� �� ������ ������ ������ �� ������, ��� ����� ��������� ��������� ��� �� 1 ���. ����� �� ������ "
            "������ � ������ � ������ ������������ ���� ��� �����. � ������ ����� ��� ��� ������������ ���� � ������� �������������� ���������� "
            "��������, ���������� �������� �� ���������� ����� �����.\n\n������ �� ��������� ���� ���������� ���������:\np - �����\nr - �����\n"
            "n - ����\nb - ����\nq - �����\nk - ������\n\n����� ������ ������������ ���������� �������, ������ - ����������.\n��� ���������� "
            "������ ��������� ������� ������� �� ������ � �������, � ����� - �� �������� ������.\n"
            "������� ����!\n",
            "����������",
            MessageBoxButtons::OK, MessageBoxIcon::Information
        );
    }

    System::Void MyForm::button5_Click(System::Object^ sender, System::EventArgs^ e) {
        this->Close();
    }

    void MyForm::button_Click(Object^ sender, EventArgs^ e) {
        MyButton^ clickedButton = static_cast<MyButton^>(sender);

        if (!isSelected) {
            clickedButton->Font = gcnew System::Drawing::Font(clickedButton->Font, FontStyle::Bold);

            src_x = clickedButton->x;
            src_y = clickedButton->y;
        }
        else {
            buttons[src_y, src_x]->Font = gcnew System::Drawing::Font(buttons[src_y, src_x]->Font, FontStyle::Regular);

            dst_x = clickedButton->x;
            dst_y = clickedButton->y;

            if (isStandartTask) {
                labelInfo->Text = "����� �����: " + totalMoves + "\n~~��� " + (params.whiteTurn ? "�����" : "������") + "~~\n";

                if (!canMove(src_x, src_y, dst_x, dst_y, params.whiteTurn)) {
                    labelInfo->Text += "\n�������� ���. ���������� ��� ���.\n";

                    
                    isSelected = !isSelected;

                    return;
                }
                else {
                    char piece = board[src_y][src_x];
                    char tempPiece = board[dst_y][dst_x];
                    // ���������� ���
                    board[src_y][src_x] = '.';
                    board[dst_y][dst_x] = piece;
                    if (isCheck(params.whiteTurn)) {
                        labelInfo->Text += "\n���������� ������� ���� - ������ ����� ��������� ���!\n";
                        // �������� ���
                        board[src_y][src_x] = piece;
                        board[dst_y][dst_x] = tempPiece;

                        
                        isSelected = !isSelected;

                        return;
                    }
                    else {
                        // �������� ���
                        board[src_y][src_x] = piece;
                        board[dst_y][dst_x] = tempPiece;
                    }
                }

                move(src_x, src_y, dst_x, dst_y);
                totalMoves += 1;
                params.whiteTurn = (!params.whiteTurn);

                labelInfo->Text = "";

                

                printBoard();

                if (isCheckmate(params.whiteTurn)) {
                    labelInfo->Text = "��� � ���! �������� " + (params.whiteTurn ? "������" : "�����") + ".\n" + "������ �� " + totalMoves + " �����\n";

                    for (int i = 0; i < BOARD_SIZE; i++)
                    {
                        for (int j = 0; j < BOARD_SIZE; j++)
                        {
                            buttons[i, j]->Enabled = false;
                        }
                    }

                    if (isSecondTask)
                    {
                        if (params.taskNumber < ETUDES_NUMBER) {
                            System::Windows::Forms::DialogResult result = System::Windows::Forms::MessageBox::Show(
                                "������� � ���������� �����?",
                                "���� ��������",
                                System::Windows::Forms::MessageBoxButtons::YesNo,
                                System::Windows::Forms::MessageBoxIcon::Question
                            );
                            params.taskNumber += 1;

                            if (result == System::Windows::Forms::DialogResult::Yes) {
                                loadTaskFromFile("etude.txt", params.taskNumber, params.whiteTurn, params.taskText);

                                isSelected = false;
                                totalMoves = 0;

                                labelInfo->Text = "����� �����: " + totalMoves + "\n~~��� " + (params.whiteTurn ? "�����" : "������") + "~~\n";
                                printBoard();

                                return;
                            }
                        }
                        else {
                            System::Windows::Forms::DialogResult result = System::Windows::Forms::MessageBox::Show(
                                "������ � ������� �����?",
                                "���� ��������",
                                System::Windows::Forms::MessageBoxButtons::YesNo,
                                System::Windows::Forms::MessageBoxIcon::Question
                            );
                            params.taskNumber = 1;

                            if (result == System::Windows::Forms::DialogResult::Yes) {
                                loadTaskFromFile("etude.txt", params.taskNumber, params.whiteTurn, params.taskText);

                                isSelected = false;
                                totalMoves = 0;

                                labelInfo->Text = "����� �����: " + totalMoves + "\n~~��� " + (params.whiteTurn ? "�����" : "������") + "~~\n";
                                printBoard();

                                return;
                            }
                        }

                        isStandartTask = false;
                        isSecondTask = false;
                    }
                    else
                    {
                        isStandartTask = false;
                    }

                    return;
                }

                if (isCheck(params.whiteTurn)) {
                    labelInfo->Text = "��������! ��� ��������� ���\n";
                }

                labelInfo->Text = "����� �����: " + totalMoves + "\n~~��� " + (params.whiteTurn ? "�����" : "������") + "~~\n" + labelInfo->Text;
            }
            else if (isFirstTask)
            {
                labelInfo->Text = msclr::interop::marshal_as<System::String^>(params.taskText);

                if (!canMove(src_x, src_y, dst_x, dst_y, params.whiteTurn)) {
                    labelInfo->Text += "\n�������� ���. ���������� ��� ���.\n";

                    isSelected = !isSelected;

                    return;
                }
                else {
                    char piece = board[src_y][src_x];
                    char tempPiece = board[dst_y][dst_x];
                    // ���������� ���
                    board[src_y][src_x] = '.';
                    board[dst_y][dst_x] = piece;
                    if (isCheck(params.whiteTurn)) {
                        labelInfo->Text += "\n���������� ������� ���� - ������ ����� ��������� ���!\n";
                        // �������� ���
                        board[src_y][src_x] = piece;
                        board[dst_y][dst_x] = tempPiece;

                        isSelected = !isSelected;

                        return;
                    }
                    else {
                        // �������� ���
                        board[src_y][src_x] = piece;
                        board[dst_y][dst_x] = tempPiece;
                    }
                }

                move(src_x, src_y, dst_x, dst_y);
                printBoard();

                if (isCheckmate(!params.whiteTurn)) {
                    labelInfo->Text = "��� � ���. �� ��������!\n";
                    if (params.taskNumber < TASK_NUMBER) {
                        System::Windows::Forms::DialogResult result = System::Windows::Forms::MessageBox::Show(
                            "������� � ��������� ������?",
                            "��� � ���. �� ��������!",
                            System::Windows::Forms::MessageBoxButtons::YesNo,
                            System::Windows::Forms::MessageBoxIcon::Question
                        );
                        params.taskNumber += 1;

                        if (result == System::Windows::Forms::DialogResult::Yes) {
                            loadTaskFromFile("mate1step.txt", params.taskNumber, params.whiteTurn, params.taskText);
                            labelInfo->Text = msclr::interop::marshal_as<System::String^>(params.taskText);

                            isSelected = false;

                            printBoard();

                            return;
                        }
                    }
                    else {
                        System::Windows::Forms::DialogResult result = System::Windows::Forms::MessageBox::Show(
                            "������ � ������ ������?",
                            "��� � ���. �� ��������!",
                            System::Windows::Forms::MessageBoxButtons::YesNo,
                            System::Windows::Forms::MessageBoxIcon::Question
                        );
                        params.taskNumber = 1;

                        if (result == System::Windows::Forms::DialogResult::Yes) {
                            loadTaskFromFile("mate1step.txt", params.taskNumber, params.whiteTurn, params.taskText);
                            labelInfo->Text = msclr::interop::marshal_as<System::String^>(params.taskText);

                            isSelected = false;

                            printBoard();

                            return;
                        }
                    }
                }
                else {
                    labelInfo->Text = "������� ��������.\n";
                    System::Windows::Forms::DialogResult result = System::Windows::Forms::MessageBox::Show(
                        "����������� ��� ���?",
                        "������� �������.",
                        System::Windows::Forms::MessageBoxButtons::YesNo,
                        System::Windows::Forms::MessageBoxIcon::Question
                    );

                    if (result == System::Windows::Forms::DialogResult::Yes) {
                        loadTaskFromFile("mate1step.txt", params.taskNumber, params.whiteTurn, params.taskText);
                        labelInfo->Text = msclr::interop::marshal_as<System::String^>(params.taskText);

                        isSelected = false;

                        printBoard();

                        return;
                    }
                }

                printBoard();

                for (int i = 0; i < BOARD_SIZE; i++)
                {
                    for (int j = 0; j < BOARD_SIZE; j++)
                    {
                        buttons[i, j]->Enabled = false;
                    }
                }

                isFirstTask = false;
            }
        }

        isSelected = !isSelected;
    }
};