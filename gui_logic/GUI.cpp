#include "GUI.h"
#include <iostream>

/*std::string GUI::getGAResult() {
    GeneticAlgorithm ga;
    std::string errorMessage = "";
    try {
        result = ga.run(costMatrix, settings);
        screen = VISUALIZATION;
        errorMessage = "";
    } catch (const std::exception &e) {
        errorMessage = e.what();
        screen = MENU;
    }
    return errorMessage;
}*/

void GUI::menu_screen(std::vector<NumberBox> &nbs, std::string &errorMessage) {
    DrawText("Assignment problem solving", 100, 50, 45, DARKGRAY);
    DrawText("Settings", 500, 130, 40, DARKGRAY);
    DrawText("Individuals", 530, 190, 22, DARKGRAY);
    DrawText("Elite individuals", 530, 231, 22, DARKGRAY);
    DrawText("Max generations", 530, 272, 22, DARKGRAY);
    DrawText("Max stagnation", 530, 313, 22, DARKGRAY);
    DrawText("Crossing chance", 530, 354, 22, DARKGRAY);
    DrawText("Mutation chance", 530, 395, 22, DARKGRAY);

    nbs[1].maxN = nbs[0].number;
    nbs[3].maxN = nbs[2].number;
    nbs[9].maxN = nbs[0].number;

    Button btnRandomMtx(60, 180, 320, 60, "Generate random matrix", 26);
    Button btnLoadMtx(60, 280, 320, 60, "Load matrix from file", 26);
    Button btnWriteMtx(60, 380, 320, 60, "Set the matrix", 26);
    Button btnExit(60, 470, 180, 50, "Exit", 26);

    std::string mutation = "Mutation: ";
    std::string mut_var[] = {"Exchange", "Subarray inversion"};
    mutation += mut_var[nbs[6].number];

    std::string crossing = "Crossing: ";
    std::string cros_var[] = {"Ordered", "Positional"};
    crossing += cros_var[nbs[7].number];

    std::string selection = "Selection: ";
    std::string sel_var[] = {"Tournament", "Stochastic Roulette"};
    selection += sel_var[nbs[8].number];

    Button btnMutation(440, 435, 340, 40, mutation, 22);
    Button btnCrossing(440, 480, 240, 40, crossing, 22);
    Button btnSelection(380, 525, 340, 40, selection, 22);

    for (NumberBox &nb: nbs) {
        nb.update();
        nb.draw();
    }

    // Делаем кнопку светлой, если мышь наведена на неё
    for (auto btn: {
             btnRandomMtx, btnLoadMtx, btnWriteMtx, btnExit,
             btnMutation, btnCrossing, btnSelection
         }) {
        btn.draw();
    }

    // Переключение режимов мутации, скрещивания и отбора
    if (btnMutation.isClicked()) {
        nbs[6].setNumber((nbs[6].number + 1) % 2);
    }
    if (btnCrossing.isClicked()) {
        nbs[7].setNumber((nbs[7].number + 1) % 2);
    }
    if (btnSelection.isClicked()) {
        nbs[8].setNumber((nbs[8].number + 1) % 2);
    }

    // Передача настроек в настройки алгоритма
    settings.populationSize = nbs[0].number;
    settings.eliteCount = nbs[1].number;
    settings.generations = nbs[2].number;
    settings.maxGenerationsWithoutImprovement = nbs[3].number;
    settings.crossoverProbability = (float) (nbs[4].number) / 100;
    settings.mutationProbability = (float) (nbs[5].number) / 100;

    settings.mutationType = static_cast<MutationType>(nbs[6].number);
    settings.crossoverType = static_cast<CrossoverType>(nbs[7].number);
    settings.selectionType = static_cast<SelectionType>(nbs[8].number);

    // Выход из приложения
    if (btnExit.isClicked()) {
        screen = EXIT;
        return;
    }

    if (btnRandomMtx.isClicked()) {
        // Здесь вызов генерации случайной матрицы
        RandomMatrixGenerator generator;
        costMatrix = generator.generate(nbs[10].number);
        stopAlgorithm = false;
        screen = LOADING;
    }
    if (btnLoadMtx.isClicked()) {
        const char *filter[] = {"*.csv"};
        const char *selectPath = tinyfd_openFileDialog(
            "Выберите файл с матрицей", "",
            1, filter,
            "*.csv", 0
        );

        if (selectPath != nullptr) {
            std::string pathString = selectPath;
            std::cout << pathString << std::endl;
            // Дальше запускаем функцию загрузки и валидации матрицы в ГА
            bool someError = false;
            try {
                MatrixReader reader;
                costMatrix = reader.read(pathString);
            } catch (const std::exception &e) {
                someError = true;
                std::cerr << "Invalid matrix format: " << e.what() << std::endl;
            }
            if (someError) {
                errorMessage = "Invalid matrix format";
            } else {
                stopAlgorithm = false;
                screen = LOADING;
            }
        } else {
            errorMessage = "";
        }
    }

    if (btnWriteMtx.isClicked()) {
        screen = MTX_CREATION;
    }

    if (screen != MENU) {
        errorMessage = "";
    }
}


void GUI::loading_screen(std::vector<NumberBox> &nbs, std::string &errorMessage) {
    if (!gaRunning) {
        gaRunning = true;
        loading_percentage = 0;
        gaFuture = std::async(std::launch::async, [this]() {
            GeneticAlgorithm ga;
            result = ga.run(costMatrix, settings, loading_percentage, stopAlgorithm);
        });
    }
    DrawText("The algorithm is running...", 140, 200, 40, DARKGRAY);
    DrawRectangle(196, 256, 408, 48, LIGHTGRAY);
    DrawRectangle(200, 260, 400 * loading_percentage, 40, DARKGRAY);
    Button btnBack(10, 5, 90, 30, "Back", 22);

    for (auto btn: {btnBack}) {
        btn.draw();
    }

    if (btnBack.isClicked()) {
        // Отключаем алгоритм и выходим
        gaRunning = false;
        stopAlgorithm = true;
        screen = MENU;
    }
}


void GUI::visualization_screen(std::vector<NumberBox> &nbs) {
    // Получение данных для визуализации
    static int currStep = 0;
    static int currIndividual = 0;

    DrawText("Visualization", 130, 5, 30, DARKGRAY);
    Button btnBack(10, 5, 90, 30, "Back", 22);
    Button btnPrevStep(100, 540, 90, 30, "<", 22);
    Button btnNextStep(200, 540, 90, 30, ">", 22);
    Button btnSkipSteps(300, 540, 90, 30, ">>", 22);
    Button btnReturnToStart(0, 540, 90, 30, "<<", 22);

    std::string showDots = " dots";
    std::string dots_var[] = {"Hide", "Show"};
    showDots = dots_var[nbs[0].number] + showDots;
    Button btnShowDots(230, 490, 150, 30, showDots, 22);

    // Переключение режимов мутации, скрещивания и отбора


    for (NumberBox &nb: nbs) {
        nb.update();
        nb.draw();
    }

    for (auto btn: {btnBack, btnNextStep, btnPrevStep, btnSkipSteps, btnReturnToStart, btnShowDots}) {
        btn.draw();
    }

    if (btnShowDots.isClicked()) {
        nbs[0].setNumber((nbs[0].number + 1) % 2);
    }

    // Отображение лучшего решения
    static Table table(3, 0, 7, 60, 60);

    // Формирование таблицы один раз
    if (currStep != 0 && result.bestIndividual.chromosome.size() > table.getRows()) {
        std::vector<std::string> firstRow;
        firstRow.push_back("Workr");
        firstRow.push_back("Work");
        firstRow.push_back("Cost");
        table.addRow(firstRow);

        for (size_t i = 0; i < result.bestIndividual.chromosome.size(); ++i) {
            std::vector<std::string> row;
            int workId = result.history[currStep - 1].bestIndividual.chromosome[i];
            row.push_back(std::to_string(i));
            row.push_back(std::to_string(workId));
            row.push_back(std::to_string(costMatrix[i][workId]));
            table.addRow(row);
            row.clear();
        }
    }
    table.draw(40, 100);

    Button btnPrevPage(220, 100, 30, 30, "/\\", 20);
    Button btnNextPage(220, 135, 30, 30, "\\/", 20);
    Button btnNextIndividual(220, 170, 30, 30, ">", 20);
    Button btnPrevIndividual(220, 205, 30, 30, "<", 20);

    if (currStep > 0) {
        btnPrevPage.draw();
        btnNextPage.draw();
        btnPrevIndividual.draw();
        btnNextIndividual.draw();
    }

    if (btnNextPage.isClicked()) {
        table.nextPage();
    }

    if (btnPrevPage.isClicked()) {
        table.prevPage();
    }

    if (btnNextIndividual.isClicked()) {
        if (currIndividual + 1 < result.history[currStep - 1].population.size()) {
            currIndividual++;
            updateTable(table, currStep, currIndividual);
        } else {
            currIndividual = 0;
            updateTable(table, currStep, currIndividual);
        }
    }

    if (btnPrevIndividual.isClicked()) {
        if (currIndividual > 0) {
            currIndividual--;
            updateTable(table, currStep, currIndividual);
        } else {
            currIndividual = result.history[currStep - 1].population.size() - 1;
            updateTable(table, currStep, currIndividual);
        }
    }

    // Характеристики промежуточного решения
    if (currStep > 0) {
        DrawText("Current best individual:", 40, 75, 25, DARKGRAY);
        Rectangle paramsArea = {220, 240, 300, 200};
        DrawText(TextFormat("Curr step: %d", currStep), paramsArea.x + 5, paramsArea.y + 5, 22, DARKGRAY);
        DrawText(TextFormat("Current best\ncost: %d", result.history[currStep - 1].bestCost), paramsArea.x + 5,
                 paramsArea.y + 50, 22, DARKGRAY);
        DrawText(TextFormat("Current\ncost: %d", result.history[currStep - 1].population[currIndividual].cost),
                 paramsArea.x + 5, paramsArea.y + 100, 22, DARKGRAY);
        DrawText(TextFormat("Current\nindividual: %d", currIndividual + 1), paramsArea.x + 5, paramsArea.y + 150,
                 22, DARKGRAY);
    }


    // Зона для графика
    Rectangle WholeGraphArea = {400, 0, 400, 600};
    Rectangle graphArea = {440, 0, 360, 560};
    DrawRectangleRec(graphArea, LIGHTGRAY);
    DrawRectangleLinesEx(graphArea, 2, BLACK);
    DrawRectangleLinesEx(WholeGraphArea, 2, BLACK);

    static std::vector<Vector2> points;
    static Scaling scale;

    // Минимальные и максимальные значения для масштабирования графика

    if (btnNextStep.isClicked() && currStep < result.history.size()) {
        currIndividual = 0;
        addPoint(points, graphArea, scale, currStep);
        updateTable(table, currStep, currIndividual);
    }

    if (btnSkipSteps.isClicked() && currStep < result.history.size()) {
        currIndividual = 0;
        for (size_t i = currStep; i < result.history.size(); ++i) {
            addPoint(points, graphArea, scale, currStep);
        }
        updateTable(table, currStep, currIndividual);
    }

    if (btnPrevStep.isClicked() && currStep > 1) {
        currIndividual = 0;
        removePoint(points, graphArea, scale, currStep);
        updateTable(table, currStep, currIndividual);
    }

    if (btnReturnToStart.isClicked() && currStep > 1) {
        currStep = 0;
        currIndividual = 0;
        scale = Scaling();
        points.clear();
        addPoint(points, graphArea, scale, currStep);
        updateTable(table, currStep, currIndividual);
    }

    // Отрисовка отрезков графика
    for (size_t i = 1; i < points.size(); ++i) {
        DrawLineEx(points[i - 1], points[i], 5, RED);
    }

    // Отрисовка точек
    for (size_t i = 0; i < points.size(); ++i) {
        if (!nbs[0].number) {
            DrawCircleV(points[i], 5, BLACK);
        } else {
            DrawCircleV(points[i], 2.5, RED);
        }
    }

    if (!points.empty()) {
        DrawCircleV(points.back(), 5, BLUE);
    }

    // Отрисовка чисел на осях
    size_t divisionsNumber = (5 > currStep) ? currStep : 5;
    divisionsNumber = (divisionsNumber < 1) ? 1 : divisionsNumber;

    float xInterval = graphArea.width / (divisionsNumber - 1);
    float yInterval = graphArea.height / (divisionsNumber - 1);

    float xStepValue = (scale.maxX - scale.minX) / (float) (divisionsNumber - 1);
    float yStepValue = (scale.maxY - scale.minY) / (float) (divisionsNumber - 1);

    float x;
    float y;
    float division;

    // Обработка случая с одной точкой
    // Отступ оси ординат от общей зоны графика
    int yAsixPadding = 5;
    if (currStep == 1) {
        y = graphArea.y + graphArea.height;
        division = 0;
        x = points[division].x;
        DrawText(TextFormat("%d", division), x, y, 20, DARKGRAY);

        // Отрисовка делений по оси ординат
        x = WholeGraphArea.x + yAsixPadding;
        y = points[0].y;
        division = scale.maxY;
        DrawText(TextFormat("%d", int(division)), x, y, 20, DARKGRAY);
    }

    // Общий случай
    for (size_t i = 0; i < divisionsNumber - 1; ++i) {
        // Отрисовка делений по оси абсцисс
        y = graphArea.y + graphArea.height;
        division = std::floor(scale.minX + i * xStepValue);
        x = points[division].x;
        DrawText(TextFormat("%d", division), x, y, 20, DARKGRAY);

        // Отрисовка делений по оси ординат
        x = WholeGraphArea.x + yAsixPadding;
        y = graphArea.y + i * yInterval + 10;
        division = scale.maxY - i * yStepValue;
        DrawText(TextFormat("%d", int(division)), x, y, 20, DARKGRAY);
    }

    // Отрисовка крайних чисел, т.к. вылезают за границы рамки
    if (divisionsNumber > 1) {
        y = graphArea.y + graphArea.height;
        division = std::floor(scale.minX + (divisionsNumber - 1) * xStepValue - 1);
        x = points[division].x;
        DrawText(TextFormat("%d", division), x, y, 20, DARKGRAY);

        // Отрисовка делений по оси ординат
        x = WholeGraphArea.x + yAsixPadding;
        y = graphArea.y + (divisionsNumber - 1) * yInterval - 20;
        division = scale.maxY - (divisionsNumber - 1) * yStepValue;
        DrawText(TextFormat("%d", int(division)), x, y, 20, DARKGRAY);
    }

    if (btnBack.isClicked()) {
        screen = MENU;
        currStep = 0;
        currIndividual = 0;
        points.clear();
        table = Table(3, 0, 7, 60, 60);
        scale = Scaling();
        result.history.clear();
        result.history.shrink_to_fit();
    }
}

void GUI::mtx_creation_screen(std::vector<NumberBox> &nbs, std::string &errorMessage) {
    DrawText("Cost matrix creation", 100, 50, 30, DARKGRAY);
    Button btnBack(10, 5, 90, 30, "Back", 22);
    Button btnContinue(630, 500, 160, 60, "Continue", 26);

    DrawText("Matrix size", 130, 110, 22, DARKGRAY);

    int mtxSize = nbs.back().number;

    DrawText("Works", 365, 365 - 41 * (mtxSize / 2.f + 1), 22, DARKGRAY);
    DrawText("W\no\nr\nk\ne\nr\ns",
             385 - 41 * (mtxSize / 2.f + 1), 295, 22, DARKGRAY);

    nbs.back().update();
    nbs.back().draw();

    int makeNextActive = 0;
    for (int i = -1; i < mtxSize; i++) {
        for (int j = -1; j < mtxSize; j++) {
            Rectangle pos = {
                400 - 41 * (mtxSize / 2.f - j),
                380 - 41 * (mtxSize / 2.f - i), 40, 40
            };

            if (i >= 0 && j >= 0) {
                NumberBox &nb = nbs[j + i * 10];
                if (nb.isBoxActive && IsKeyPressed(KEY_ENTER)) {
                    makeNextActive = 2;
                }
                nb.rect = pos;
                nb.update();
                if (makeNextActive == 2) {
                    makeNextActive--;
                } else if (makeNextActive == 1) {
                    nb.isBoxActive = true;
                    makeNextActive--;
                }
                nb.draw();
                continue;
            }

            // Отображение номеров столбцов и колонок
            char c[] = {'0'};
            if (i == -1 && j != -1) {
                c[0] += j;
                DrawText(c, pos.x + 15, pos.y + 15, 22, LIGHTGRAY);
            }
            if (i != -1 && j == -1) {
                c[0] += i;
                DrawText(c, pos.x + 15, pos.y + 10, 22, LIGHTGRAY);
            }
        }
    }

    for (auto btn: {btnBack, btnContinue}) {
        btn.draw();
    }

    if (btnBack.isClicked()) {
        // Отменяем создание матрицы
        screen = MENU;
    }
    if (btnContinue.isClicked()) {
        // Передача данных из полей в матрицу
        costMatrix = std::vector<std::vector<int> >(mtxSize, std::vector<int>(mtxSize));
        for (size_t i = 0; i < mtxSize; ++i) {
            for (size_t j = 0; j < mtxSize; ++j) {
                costMatrix[i][j] = nbs[i * nbs.back().maxN + j].number;
            }
        }

        stopAlgorithm = false;
        screen = LOADING;
    }
}


void GUI::addPoint(std::vector<Vector2> &points, const Rectangle &graphArea, Scaling &scale, int &currStep) {
    int x = result.history[currStep].generationNumber + 1;
    int y = result.history[currStep].bestCost;


    scale.maxX = currStep + 1;
    scale.minY = std::min(scale.minY, y);
    scale.maxY = std::max(scale.maxY, y);

    float xScale = (graphArea.width - 20) / (float) (scale.maxX - scale.minX + 1);
    float yScale = (graphArea.height - 20) / (float) (scale.maxY - scale.minY + 1);

    points.push_back({0, 0});

    // Перерасчет расположения точек
    for (size_t i = 0; i < points.size(); ++i) {
        x = result.history[i].generationNumber + 1;
        y = result.history[i].bestCost;

        points[i].x = graphArea.x + 10 + (x - scale.minX) * xScale;
        points[i].y = graphArea.y + graphArea.height - 10 - (y - scale.minY) * yScale;
    }

    if (points.size() == 1) {
        points[0].x = graphArea.x + 10 + xScale;
        points[0].y = graphArea.y + graphArea.height - 10 - yScale / 2;
    }

    currStep++;
}

void GUI::removePoint(std::vector<Vector2> &points, const Rectangle &graphArea, Scaling &scale, int &currStep) {
    currStep--;
    if (result.history[currStep].bestCost == scale.minY) scale.minY = std::numeric_limits<int>::max();
    if (result.history[currStep].bestCost == scale.maxY) scale.maxY = 0;

    points.pop_back();


    scale.maxX = currStep;
    scale.minY = std::min(scale.minY, result.history[currStep - 1].bestCost);
    scale.maxY = std::max(scale.maxY, result.history[currStep - 1].bestCost);

    float xScale = (graphArea.width - 20) / (float) (scale.maxX - scale.minX + 1);
    float yScale = (graphArea.height - 20) / (float) (scale.maxY - scale.minY + 1);

    // Перерасчет расположения точек
    for (size_t i = 0; i < points.size(); ++i) {
        int x = result.history[i].generationNumber + 1;
        int y = result.history[i].bestCost;

        points[i].x = graphArea.x + 10 + (x - scale.minX) * xScale;
        points[i].y = graphArea.y + graphArea.height - 10 - (y - scale.minY) * yScale;
    }

    if (points.size() == 1) {
        points[0].x = graphArea.x + 10 + xScale;
        points[0].y = graphArea.y + graphArea.height - 10 - yScale / 2;
    }
}

void GUI::updateTable(Table &table, int &currStep, int &currIndividual) {
    for (size_t i = 1; i < table.getRows(); ++i) {
        std::vector<std::string> row;
        int workId = result.history[currStep - 1].population[currIndividual].chromosome[i - 1];
        row.push_back(std::to_string(i - 1));
        row.push_back(std::to_string(workId));
        row.push_back(std::to_string(costMatrix[i - 1][workId]));
        table.setRow(i, row);
        row.clear();
    }
}
