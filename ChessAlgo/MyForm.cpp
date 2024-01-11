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

    // Хранение расположения фигур на шахматной доске
    char board[BOARD_SIZE][BOARD_SIZE];

    // Функция для загрузки задачи из файла
    void MyForm::loadTaskFromFile(string filename, int taskNumber, bool& whiteTurn, string& text) {
        ifstream file(filename);
        if (!file.is_open()) {
            labelText->Text = "Ошибка открытия файла\n";
            return;
        }
        setlocale(LC_ALL, "rus");

        // Считываем задачи до нужной нам
        for (int i = 1; i < taskNumber; ++i) {
            string line;
            getline(file, line); // Текст задачи
            getline(file, line); // Чей ход
            // Шахматная доска
            for (int row = 0; row < BOARD_SIZE; ++row) {
                getline(file, line);
                memcpy(board[row], line.c_str(), BOARD_SIZE);
            }
            getline(file, line); // Пустая строка между задачами
        }

        // Считываем нужную задачу
        getline(file, text); // Текст задачи
        string turnString;
        getline(file, turnString); // Чей ход
        whiteTurn = (turnString == "Ход белых");
        // Шахматная доска
        for (int row = 0; row < BOARD_SIZE; ++row) {
            string line;
            getline(file, line);
            memcpy(board[row], line.c_str(), BOARD_SIZE);
        }

        // Закрываем файл
        file.close();
    }

    // Функция для вывода расположения фигур на доске на экран
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

    // Функция для определения, может ли фигура сходить на указанную позицию
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
        case 'p': // Пешка
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

        case 'r': // Ладья
            if (deltaX == 0 || deltaY == 0) {
                return isPathClear(fromX, fromY, toX, toY);
            }
            return false;

        case 'n': // Конь
            return (deltaX == 1 && deltaY == 2) || (deltaX == 2 && deltaY == 1);

        case 'b': // Слон
            if (deltaX == deltaY) {
                return isPathClear(fromX, fromY, toX, toY);
            }
            return false;

        case 'q': // Ферзь
            if ((deltaX == 0) != (deltaY == 0) || deltaX == deltaY) {
                return isPathClear(fromX, fromY, toX, toY);
            }
            return false;

        case 'k': // Король
            return (deltaX <= 1 && deltaY <= 1);

        default:
            return false;
        }
    }

    // Функция для перемещения фигуры на заданную позицию
    void MyForm::move(int fromX, int fromY, int toX, int toY) {
        board[toY][toX] = board[fromY][fromX];
        board[fromY][fromX] = '.';
    }

    // Функция для проверки шаха
    bool MyForm::isCheck(bool whiteKing) {
        int kingX = -1;
        int kingY = -1;

        // Ищем позицию короля
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
            // Нет короля на доске
            return false;
        }

        // Проверка, может ли кто-то из соперников добраться до короля
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

    // Функция для проверки матa
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

                        // Сохраняем текущее состояние доски
                        char tempPiece = board[newY][newX];

                        // Симулируем ход
                        board[y][x] = '.';
                        board[newY][newX] = piece;

                        if (!isCheck(whiteKing)) {
                            // Отменяем ход
                            board[y][x] = piece;
                            board[newY][newX] = tempPiece;
                            return false;
                        }

                        // Отменяем ход
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

        labelInfo->Text = "Всего ходов: " + totalMoves + "\n~~Ход " + (params.whiteTurn ? "белых" : "черных") + "~~\n";
        printBoard();

        groupBox1->Visible = true;
    }

    System::Void MyForm::button1_Click(System::Object^ sender, System::EventArgs^ e) {
        ChessAlgo::InputBox inputBox(
            "Мат в 1 ход",
            "Для решения задач Вам потребуется поставить мат королю соперника за 1 ход.\nВведите номер задачи от 1 до " + TASK_NUMBER + ": ",
            &params.taskNumber
            );
        inputBox.ShowDialog();

        while (params.taskNumber < 1 || params.taskNumber > TASK_NUMBER) {
            inputBox.changeText(
                "Мат в 1 ход", 
                "Неверно введен номер задачи. Попробуйте снова.\nВведите номер задачи от 1 до " + TASK_NUMBER + ": ");
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
            "Этюд",
            "В этюде Вы можете постараться поставить другому игроку мат при заданной ситуации за наименьшее число ходов\nВведите номер этюда от 1 до " + ETUDES_NUMBER + ": ",
            &params.taskNumber
        );
        inputBox.ShowDialog();

        while (params.taskNumber < 1 || params.taskNumber > TASK_NUMBER) {
            inputBox.changeText(
                "Этюд",
                "Неверно введен номер этюда. Попробуйте снова.\nВведите номер этюда от 1 до " + ETUDES_NUMBER + ": ");
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
            "Добро пожаловать в Шахматы!\nЗдесь Вы можете решать задачи на логику, где нужно поставить сопернику мат за 1 ход. Также Вы можете "
            "играть с другом в режиме классической игры или этюда. В режиме этюда для Вас подготовлено поле с заранее расставленными шахматными "
            "фигурами, попробуйте победить за наименьшее число ходов.\n\nФигуры на шахматном поле обозначены символами:\np - пешка\nr - ладья\n"
            "n - конь\nb - слон\nq - ферзь\nk - король\n\nБелые фигуры обозначаются заглавными буквами, черные - прописными.\nДля корректной "
            "работы программы нажмите сначала на клетку с фигурой, а затем - на конечную клетку.\n"
            "Удачной игры!\n",
            "Информация",
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
                labelInfo->Text = "Всего ходов: " + totalMoves + "\n~~Ход " + (params.whiteTurn ? "белых" : "черных") + "~~\n";

                if (!canMove(src_x, src_y, dst_x, dst_y, params.whiteTurn)) {
                    labelInfo->Text += "\nНеверный ход. Попробуйте еще раз.\n";

                    
                    isSelected = !isSelected;

                    return;
                }
                else {
                    char piece = board[src_y][src_x];
                    char tempPiece = board[dst_y][dst_x];
                    // Симулируем ход
                    board[src_y][src_x] = '.';
                    board[dst_y][dst_x] = piece;
                    if (isCheck(params.whiteTurn)) {
                        labelInfo->Text += "\nНевозможно сходить сюда - королю будет поставлен шах!\n";
                        // Отменяем ход
                        board[src_y][src_x] = piece;
                        board[dst_y][dst_x] = tempPiece;

                        
                        isSelected = !isSelected;

                        return;
                    }
                    else {
                        // Отменяем ход
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
                    labelInfo->Text = "Шах и мат! Победили " + (params.whiteTurn ? "чёрные" : "белые") + ".\n" + "Победа за " + totalMoves + " ходов\n";

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
                                "Перейти к следующему этюду?",
                                "Этюд завершен",
                                System::Windows::Forms::MessageBoxButtons::YesNo,
                                System::Windows::Forms::MessageBoxIcon::Question
                            );
                            params.taskNumber += 1;

                            if (result == System::Windows::Forms::DialogResult::Yes) {
                                loadTaskFromFile("etude.txt", params.taskNumber, params.whiteTurn, params.taskText);

                                isSelected = false;
                                totalMoves = 0;

                                labelInfo->Text = "Всего ходов: " + totalMoves + "\n~~Ход " + (params.whiteTurn ? "белых" : "черных") + "~~\n";
                                printBoard();

                                return;
                            }
                        }
                        else {
                            System::Windows::Forms::DialogResult result = System::Windows::Forms::MessageBox::Show(
                                "Начать с первого этюда?",
                                "Этюд завершен",
                                System::Windows::Forms::MessageBoxButtons::YesNo,
                                System::Windows::Forms::MessageBoxIcon::Question
                            );
                            params.taskNumber = 1;

                            if (result == System::Windows::Forms::DialogResult::Yes) {
                                loadTaskFromFile("etude.txt", params.taskNumber, params.whiteTurn, params.taskText);

                                isSelected = false;
                                totalMoves = 0;

                                labelInfo->Text = "Всего ходов: " + totalMoves + "\n~~Ход " + (params.whiteTurn ? "белых" : "черных") + "~~\n";
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
                    labelInfo->Text = "Внимание! Вам поставлен шах\n";
                }

                labelInfo->Text = "Всего ходов: " + totalMoves + "\n~~Ход " + (params.whiteTurn ? "белых" : "черных") + "~~\n" + labelInfo->Text;
            }
            else if (isFirstTask)
            {
                labelInfo->Text = msclr::interop::marshal_as<System::String^>(params.taskText);

                if (!canMove(src_x, src_y, dst_x, dst_y, params.whiteTurn)) {
                    labelInfo->Text += "\nНеверный ход. Попробуйте еще раз.\n";

                    isSelected = !isSelected;

                    return;
                }
                else {
                    char piece = board[src_y][src_x];
                    char tempPiece = board[dst_y][dst_x];
                    // Симулируем ход
                    board[src_y][src_x] = '.';
                    board[dst_y][dst_x] = piece;
                    if (isCheck(params.whiteTurn)) {
                        labelInfo->Text += "\nНевозможно сходить сюда - королю будет поставлен шах!\n";
                        // Отменяем ход
                        board[src_y][src_x] = piece;
                        board[dst_y][dst_x] = tempPiece;

                        isSelected = !isSelected;

                        return;
                    }
                    else {
                        // Отменяем ход
                        board[src_y][src_x] = piece;
                        board[dst_y][dst_x] = tempPiece;
                    }
                }

                move(src_x, src_y, dst_x, dst_y);
                printBoard();

                if (isCheckmate(!params.whiteTurn)) {
                    labelInfo->Text = "Шах и мат. Вы победили!\n";
                    if (params.taskNumber < TASK_NUMBER) {
                        System::Windows::Forms::DialogResult result = System::Windows::Forms::MessageBox::Show(
                            "Перейти к следующей задаче?",
                            "Шах и мат. Вы победили!",
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
                            "Начать с первой задачи?",
                            "Шах и мат. Вы победили!",
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
                    labelInfo->Text = "Решение неверное.\n";
                    System::Windows::Forms::DialogResult result = System::Windows::Forms::MessageBox::Show(
                        "Попробовать еще раз?",
                        "Решение неверно.",
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