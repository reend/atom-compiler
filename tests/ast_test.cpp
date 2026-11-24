#include "../frontend/ast.hpp"

#include <iostream>
#include <utility>

int main()
{
    auto two = std::make_unique<IntegerExpr>();
    two->value = 2;
    two->line = 1;
    two->column = 1;

    auto three = std::make_unique<IntegerExpr>();
    three->value = 3;
    three->line = 1;
    three->column = 5;

    auto four = std::make_unique<IntegerExpr>();
    four->value = 4;
    four->line = 1;
    four->column = 9;

    auto mul = std::make_unique<BinaryExpr>();

    mul->op = TokenKind::Star;
    mul->line = 1;
    mul->column = 7;

    mul->left = std::move(three);
    mul->right = std::move(four);

    auto root = std::make_unique<BinaryExpr>();

    root->op = TokenKind::Plus;
    root->line = 1;
    root->column = 3;

    root->left = std::move(two);
    root->right = std::move(mul);

    std::cout << tokenKindToString(root->op) << "\n";

    std::cout << "Left position: "
            << root->left->line << ":"
            << root->left->column << "\n";

    std::cout << "Right position: "
            << root->right->line << ":"
            << root->right->column << "\n";

    std::cout << std::boolalpha;

    std::cout << "two is empty: " << (two == nullptr) << "\n";
    std::cout << "three is empty: " << (three == nullptr) << "\n";
    std::cout << "four is empty: " << (four == nullptr) << "\n";
    std::cout << "mul is empty: " << (mul == nullptr) << "\n";

    auto* number =
    dynamic_cast<IntegerExpr*>(root->left.get());

    if (number != nullptr)
    {
        std::cout << "Left value: "
                << number->value << "\n";
    }

    auto* binary =
        dynamic_cast<BinaryExpr*>(root->right.get());

    if (binary != nullptr)
    {
        std::cout << "Right operator: "
                << tokenKindToString(binary->op) << "\n";
    }

    auto* invalid =
    dynamic_cast<BoolExpr*>(root->left.get());

    if (invalid == nullptr)
    {
        std::cout << "Invalid cast detected\n";
    }

    return 0;
}