#include <iostream>
#include <vector>
#include <stack>

class Node;

class Desc
{
  public:
    Node *NextNode = nullptr;
    char AcceptValue;
    bool MatchAny;
    Desc(char acceptValue, bool matchAny, Node *nextNode)
        : AcceptValue(acceptValue), MatchAny(matchAny), NextNode(nextNode) {}

    ~Desc() {}
};

class Node
{
  public:
    std::vector<Desc> AcceptState;
    bool CanBeEnd = false;

    ~Node() {}
};

enum Type
{
    Begin,
    Word,
    LeftBrace,
    Or,
    RightBrace,
    Repeat,
    Escape,
    Any,
    AnyOnce,
    End
};

class DFA
{
  private:
    Node *start = new Node;
    std::string record = "";
    Type ProcessChar(char t)
    {
        switch (t)
        {
        case '^':
            return Begin;
        case '$':
            return End;
        case '(':
            return LeftBrace;
        case ')':
            return RightBrace;
        case '|':
            return Or;
        case '\\':
            return Escape;
        case '*':
            return Repeat;
        case '?':
            return AnyOnce;
        case '.':
            return Any;
        default:
            return Word;
        }
    }

    void PutDesc(Desc desc)
    {
        printf("--> { value: %c, any: %d }: ", desc.AcceptValue, desc.MatchAny);
    }

    void PutBlankSpace(int count)
    {
        for (auto i = 0; i < count; i++)
        {
            printf("  ");
        }
    }

    bool MatchProcess(std::string str, int index, Node *node, int step, bool emptyMatched = false)
    {
        if (index >= str.length() || node == nullptr || node->AcceptState.size() == 0)
        {
            PutBlankSpace(step - 1);
            printf("Mismatched\n");
            return false;
        }
        PutBlankSpace(step);
        printf("Matching '%c' at position %d: \n", str[index], index);
        auto result = false;
        for (auto state : node->AcceptState)
        {
            PutBlankSpace(step);
            PutDesc(state);
            if (state.MatchAny || state.AcceptValue == str[index])
            {
                if (record.length() <= step)
                {
                    record += str[index];
                }
                else
                {
                    record[step] = str[index];
                }
                printf("Succeeded\n");
                if (node->CanBeEnd)
                {
                    matches.push_back(record.substr(0, step + 1));
                    PutBlankSpace(step + 1);
                    printf("Matched\n");
                    result = true;
                }
                result = MatchProcess(str, index + 1, state.NextNode, step + 1) || result;
            }
            else
            {
                printf("Failed\n");
            }
        }
        return result;
    }

  public:
    std::vector<std::string> matches;
    DFA(std::string patterns)
    {
        if (patterns.empty())
            throw "EmptyPatterns";
        auto currentNode = start;
        auto escape = false;
        auto lastWord = patterns[0];
        auto len = patterns.length();
        for (auto i = 0; i < len; i++)
        {
            auto nextNode = new Node;
            switch (ProcessChar(patterns[i]))
            {
            case Begin:
            case End:
            case LeftBrace:
            case RightBrace:
            case Or:
                throw "NotImplemented";

            case Word:
            {
                lastWord = patterns[i];
                currentNode->CanBeEnd = i == len - 1;
                Desc desc(patterns[i], false, nextNode);
                currentNode->AcceptState.push_back(desc);
                break;
            }
            case Any:
            {
                currentNode->CanBeEnd = i == len - 1;
                if (escape)
                {
                    lastWord = patterns[i];
                    escape = false;
                    Desc desc(patterns[i], false, nextNode);
                    currentNode->AcceptState.push_back(desc);
                    break;
                }
                {
                    Desc self(patterns[i], true, currentNode);
                    currentNode->AcceptState.push_back(self);
                    Desc desc(patterns[i], true, nextNode);
                    currentNode->AcceptState.push_back(desc);
                    continue;
                }
            }
            case AnyOnce:
            {
                currentNode->CanBeEnd = i == len - 1;
                if (escape)
                {
                    lastWord = patterns[i];
                    escape = false;
                    Desc desc(patterns[i], false, nextNode);
                    currentNode->AcceptState.push_back(desc);
                    break;
                }
                Desc desc(patterns[i], true, nextNode);
                currentNode->AcceptState.push_back(desc);
                break;
            }
            case Escape:
            {
                escape = true;
                break;
            }
            case Repeat:
            {
                currentNode->CanBeEnd = i == len - 1;
                Desc desc(lastWord, false, currentNode);
                currentNode->AcceptState.push_back(desc);
                continue;
            }
            }
            currentNode = nextNode;
        }
    }

    bool Match(std::string str)
    {
        matches.clear();
        printf("Input str: %s\n", str.c_str());
        auto len = str.length();
        auto result = false;
        for (auto i = 0; i < len; i++)
        {
            result = MatchProcess(str, i, start, 0) || result;
        }
        return result;
    }
};

int main()
{
    DFA dfa(".e2*.3.??7??");
    if (dfa.Match("sdnu92c9w2c9e22436i308877dis"))
    {
        printf("\n%llu matches:\n", dfa.matches.size());
        for (auto match : dfa.matches)
            printf("%s\n", match.c_str());
    }
    else
        printf("No match!\n");
    getchar();
    return 0;
}