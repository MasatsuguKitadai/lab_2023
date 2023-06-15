#include <boost/progress.hpp>
#include <unistd.h>

int main()
{
    const int MAX = 1000;

    boost::progress_display show_progress(MAX);
    for (int i = 0; i < MAX; ++i)
    {
        ++show_progress;
        printf("%d\n", i);
    }
    return 0;
}