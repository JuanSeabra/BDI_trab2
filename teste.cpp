#include <string>
#include <iostream>
using std::cout;
using std::cin;
using std::string;

#include <boost/tokenizer.hpp>
using boost::tokenizer;
using boost::escaped_list_separator;

typedef tokenizer<escaped_list_separator<char> > so_tokenizer;

int main()
{
    string s("\"13986\";\"Parallelizing Convolutional Neural Networks on Intel $$^{textregistered }$$ Many Integrated Core Architecture.\";\"2015\";\"Junjie Liu|Haixia Wang|Dongsheng Wang|Yuan Gao|Zuofeng Li\";\"1\";\"2016-08-21 09:53:00\";\"Parallelizing Convolutional Neural Networks on Intel^{ textregistered} Many Integrated Core Architecture. J Liu, H Wang, D Wang, Y Gao, Z Li -   Architecture of Computing Systems, 2015 - Springer. Abstract Convolutional neural networks (CNNs) are state-of-the-art machine learning  algorithm in low-resolution vision tasks and are widely applied in many applications.  However, the training process of them is very time-consuming. As a result, many  ..\"");

    so_tokenizer tok(s, escaped_list_separator<char>('\\', ';', '\"'));
    for(so_tokenizer::iterator beg=tok.begin(); beg!=tok.end(); ++beg)
    {
        cout << *beg << "\n";
    }

    return 0;
}
