#include <stdio.h>
#include <ctype.h>
#include <string.h> // for strlen
#include <sys/socket.h>
#include <arpa/inet.h> // for inet_addr
#include <unistd.h>    // for write
#include <time.h>      //for getting system time
                       

#define NANO 1000000000L
#define PORTNUMBER 8888
void operation(char *str);
int timespec2str(char *buf, uint len, struct timespec *ts);

int errorflag = 0;
char message[1024] = {0};
int mcounter = 0;
char *errorMessage;
int new_socket;

int main(int argc, char *argv[])
{
    int socket_desc, c;
    struct sockaddr_in server, client;

    char *hello = "Hello from server";

    // Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1)
    {
        puts("Could not create socket");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORTNUMBER);

    // Bind
    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        puts("Bind failed");
        return 1;
    }
    puts("Socket is binded");

    // Listen
    listen(socket_desc, 3);

    // Accept and incoming connection
    puts("Waiting for incoming connections...");

    c = sizeof(struct sockaddr_in);
    while (new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c))
    {
        memset(message, 0, sizeof message);
        errorflag = 0;
        mcounter = 0;
        char *splitted;
        puts("Connection accepted");
        char buffer[1024] = {0};
        read(new_socket, buffer, 1024);
        splitted = strtok(buffer, " ");
        
        if ((strcmp(splitted, "") != 0) && (strcmp(splitted, " ") != 0))
        {
            if (strcmp(splitted, "GET_DATE") == 0)
            {
                splitted = strtok(NULL, " ");
                while (splitted != NULL)
                {
                    operation(splitted);
                    splitted = strtok(NULL, " ");
                }
                
                if (errorflag == 0)
                {
                    message[mcounter] = '\0';
                    write(new_socket, message, strlen(message));
                }
            }
            else
            {
                errorMessage = "INCORRECT REQUEST\n";
                write(new_socket, errorMessage, strlen(errorMessage));
            }
        }
    }
    close(socket_desc);
    close(new_socket);
    return 0;
}

void operation(char *str)
{
    struct tm *loctime;
    time_t t;
    char buff[1024];
    int opencommand = 0; // When % is come next character must be a command character otherwise it becomes invalid syntax.
    /* Get the current time. */
    t = time(NULL);

    /* Convert it to local time representation. */
    loctime = localtime(&t);

    int twodotcounter = 0; //to print %:z, %::z, %:::z
    int letter = 0;

    if (strcmp(str, " ") != 0)
    {
        int i, j;
        for (i = 0; i < strlen(str); i++)
        {

            if (opencommand == 1 | twodotcounter > 0)
            {
                if (str[i] == '%')
                {
                    strftime(buff, 1024, "%%", loctime);
                    letter = 1;
                }
                else if (str[i] == 'a') //%a  locale's abbreviated weekday name (e.g., Sun)
                {
                    strftime(buff, 1024, "%a", loctime);
                    letter = 1;
                }
                else if (str[i] == 'A') //   %A     locale's full weekday name (e.g., Sunday)
                {
                    strftime(buff, 1024, "%A", loctime);
                    letter = 1;
                }
                else if (str[i] == 'b' | str[i] == 'h') //   %b     locale's abbreviated month name (e.g., Jan)
                {
                    strftime(buff, 1024, "%b", loctime);
                    letter = 1;
                }
                else if (str[i] == 'B') // %B     locale's full month name (e.g., January)
                {
                    strftime(buff, 1024, "%B", loctime);
                    letter = 1;
                }
                else if (str[i] == 'c') //  %c     locale's date and time (e.g., Thu Mar  3 23:05:25 2005)
                {
                    strftime(buff, 1024, "%c", loctime);
                    letter = 1;
                }
                else if (str[i] == 'C') //  %C     century; like %Y, except omit last two digits (e.g., 20)
                {
                    strftime(buff, 1024, "%Y", loctime);
                    for (j = 0; j < 2; j++)
                    {
                        message[mcounter] = buff[j];
                        mcounter++;
                    }
                    opencommand = 0;
                }
                else if (str[i] == 'd') // %d     day of month (e.g., 01)
                {
                    strftime(buff, 1024, "%d", loctime);
                    letter = 1;
                }
                else if (str[i] == 'D') //  %D     date; same as %m/%d/%y
                {
                    strftime(buff, 1024, "%m/%d/%y", loctime);
                    letter = 1;
                }
                else if (str[i] == 'e') //       %e     day of month, space padded; same as %_d
                {
                    strftime(buff, 1024, "%e", loctime);
                    letter = 1;
                }
                else if (str[i] == 'F') //      %F     full date; like %+4Y-%m-%d
                {
                    strftime(buff, 1024, "%Y-%m-%d", loctime);
                    letter = 1;
                }
                else if (str[i] == 'g') //   %g     last two digits of year of ISO week number (see %G)
                {
                    strftime(buff, 1024, "%g", loctime);
                    letter = 1;
                }
                else if (str[i] == 'G') //    %G     year of ISO week number (see %V); normally useful only with %V
                {
                    strftime(buff, 1024, "%G", loctime);
                    letter = 1;
                }
                else if (str[i] == 'H') //    %H     hour (00..23)
                {
                    strftime(buff, 1024, "%H", loctime);
                    letter = 1;
                }
                else if (str[i] == 'I') //       %I     hour (01..12)
                {
                    strftime(buff, 1024, "%I", loctime);
                    letter = 1;
                }
                else if (str[i] == 'j') //      %j     day of year (001..366)
                {
                    strftime(buff, 1024, "%j", loctime);
                    letter = 1;
                }
                else if (str[i] == 'k') //   %k     hour, space padded ( 0..23); same as %_H
                {
                    strftime(buff, 1024, "%_H", loctime);
                    letter = 1;
                }
                else if (str[i] == 'l') //   %l     hour, space padded ( 1..12); same as %_I
                {
                    strftime(buff, 1024, "%_I", loctime);
                    letter = 1;
                }
                else if (str[i] == 'm') //       %m     month (01..12)
                {
                    strftime(buff, 1024, "%m", loctime);
                    letter = 1;
                }
                else if (str[i] == 'M') //     %M     minute (00..59)
                {
                    strftime(buff, 1024, "%M", loctime);
                    letter = 1;
                }
                else if (str[i] == 'n') //    %n     a newline
                {
                    message[mcounter] = '\n';
                    mcounter++;
                    opencommand = 0;
                }
                else if (str[i] == 'N') //%N     nanoseconds (000000000..999999999)
                {
                    clockid_t clk_id = CLOCK_REALTIME;
                    char timestr[10];
                    struct timespec ts, res;
                    clock_getres(clk_id, &res);
                    clock_gettime(clk_id, &ts);
                    if (timespec2str(timestr, sizeof(timestr), &ts) == 0)
                    {
                        unsigned long resol = res.tv_sec * NANO + res.tv_nsec;
                    }
                    for (j = 0; j < strlen(timestr); j++)
                    {
                        message[mcounter] = timestr[j];
                        mcounter++;
                    }

                    opencommand = 0;
                }
                else if (str[i] == 'p') //    %p     locale's equivalent of either AM or PM; blank if not known
                {
                    strftime(buff, 1024, "%p", loctime);
                    letter = 1;
                }
                else if (str[i] == 'P') //   %P     like %p, but lower case
                {
                    strftime(buff, 1024, "%P", loctime);
                    letter = 1;
                }
                else if (str[i] == 'q') //  %q     quarter of year (1..4)
                {
                    char q;
                    strftime(buff, 1024, "%m", loctime);
                    if ((strcmp(buff, "01") == 0) | (strcmp(buff, "02") == 0) | (strcmp(buff, "03") == 0))
                        q = 1 + '0';
                    else if ((strcmp(buff, "04") == 0) | (strcmp(buff, "05") == 0) | (strcmp(buff, "06") == 0))
                        q = 2 + '0';
                    else if ((strcmp(buff, "07") == 0) | (strcmp(buff, "08") == 0) | (strcmp(buff, "09") == 0))
                        q = 3 + '0';
                    else
                        q = 4 + '0';
                    message[mcounter] = q;
                    mcounter++;
                    opencommand = 0;
                }
                else if (str[i] == 'r') // %r     locale's 12-hour clock time (e.g., 11:11:04 PM)
                {
                    strftime(buff, 1024, "%r", loctime);
                    letter = 1;
                }
                else if (str[i] == 'R') //  %R     24-hour hour and minute; same as %H:%M
                {
                    strftime(buff, 1024, "%H:%M", loctime);
                    letter = 1;
                }
                else if (str[i] == 's') //    %s     seconds since 1970-01-01 00:00:00 UTC
                {
                    strftime(buff, 1024, "%s", loctime);
                    letter = 1;
                }
                else if (str[i] == 'S') //    %S     second (00..60)
                {
                    strftime(buff, 1024, "%S", loctime);
                    letter = 1;
                }
                else if (str[i] == 't') //    %t     a tab
                {
                    message[mcounter] = '\t';
                    mcounter++;
                    opencommand = 0;
                }
                else if (str[i] == 'T') //     %T     time; same as %H:%M:%S
                {
                    strftime(buff, 1024, "%H:%M:%S", loctime);
                    letter = 1;
                }
                else if (str[i] == 'u') //   %u     day of week (1..7); 1 is Monday
                {
                    strftime(buff, 1024, "%u", loctime);
                    letter = 1;
                }
                else if (str[i] == 'U') //    %U     week number of year, with Sunday as first day of week (00..53)
                {
                    strftime(buff, 1024, "%U", loctime);
                    letter = 1;
                }
                else if (str[i] == 'V') //   %V     ISO week number, with Monday as first day of week (01..53)
                {
                    strftime(buff, 1024, "%V", loctime);
                    letter = 1;
                }
                else if (str[i] == 'w') //  %w     day of week (0..6); 0 is Sunday
                {
                    strftime(buff, 1024, "%w", loctime);
                    letter = 1;
                }
                else if (str[i] == 'W') //    %W     week number of year, with Monday as first day of week (00..53)
                {
                    strftime(buff, 1024, "%W", loctime);
                    letter = 1;
                }
                else if (str[i] == 'x') //  %x     locale's date representation (e.g., 12/31/99)
                {
                    strftime(buff, 1024, "%x", loctime);
                    letter = 1;
                }
                else if (str[i] == 'X') //     %X     locale's time representation (e.g., 23:13:48)
                {
                    strftime(buff, 1024, "%X", loctime);
                    letter = 1;
                }
                else if (str[i] == 'y') // %y     last two digits of year (00..99)
                {
                    strftime(buff, 1024, "%y", loctime);
                    letter = 1;
                }
                else if (str[i] == 'Y') //%Y     year
                {
                    strftime(buff, 1024, "%Y", loctime);
                    letter = 1;
                }
                else if (str[i] == 'z')
                {
                    if (twodotcounter == 0) //  %z     +hhmm numeric time zone (e.g., -0400)
                    {
                        strftime(buff, 1024, "%z", loctime);
                        letter = 1;
                    }
                    else if (twodotcounter == 1) //  %:z    +hh:mm numeric time zone (e.g., -04:00)
                    {
                        strftime(buff, 1024, "%z", loctime);
                        for (j = 0; j < 3; j++)
                        {
                            message[mcounter] = buff[j];
                            mcounter++;
                        }
                        message[mcounter] = ':';
                        mcounter++;
                        for (j; j < strlen(buff); j++)
                        {
                            message[mcounter] = buff[j];
                            mcounter++;
                        }
                        opencommand = 0;
                        twodotcounter = 0;
                    }
                    else if (twodotcounter == 2) //%::z   +hh:mm:ss numeric time zone (e.g., -04:00:00)
                    {
                        strftime(buff, 1024, "%z", loctime);
                        for (j = 0; j < 3; j++)
                        {
                            message[mcounter] = buff[j];
                            mcounter++;
                        }
                        message[mcounter] = ':';
                        mcounter++;
                        for (j; j < strlen(buff); j++)
                        {
                            message[mcounter] = buff[j];
                            mcounter++;
                        }
                        message[mcounter] = ':';
                        mcounter++;
                        message[mcounter] = 0 + '0';
                        mcounter++;
                        message[mcounter] = 0 + '0';
                        mcounter++;
                        opencommand = 0;
                        twodotcounter = 0;
                    }
                    else if (twodotcounter == 3) //%:::z  numeric time zone with : to necessary precision (e.g.,-04, +05:30)
                    {
                        strftime(buff, 1024, "%z", loctime);
                        for (j = 0; j < 3; j++)
                        {
                            message[mcounter] = buff[j];
                            mcounter++;
                        }
                        opencommand = 0;
                        twodotcounter = 0;
                    }
                    else
                    {
                        errorflag = 1;
                        errorMessage = "INCORRECT REQUEST\n";
                        write(new_socket, errorMessage, strlen(errorMessage));
                        break;
                    }
                }
                else if (str[i] == 'Z') //  %Z     alphabetic time zone abbreviation (e.g., EDT)
                {
                    strftime(buff, 1024, "%Z", loctime);
                    letter = 1;
                }

                else if (str[i] == ':')
                    twodotcounter++;
                else
                {
                    errorflag = 1;
                    errorMessage = "INCORRECT REQUEST\n";
                    write(new_socket, errorMessage, strlen(errorMessage));
                    break;
                }

                if (letter == 1)
                {
                    for (j = 0; j < strlen(buff); j++)
                    {
                        message[mcounter] = buff[j];
                        mcounter++;
                    }
                    opencommand = 0;
                    letter = 0;
                }
            }
            else if (str[i] == '%')
            {
                opencommand = 1;
            }
            else
            {
                message[mcounter] = str[i];
                mcounter++;
            }
        }
    }
}

//https://www.it-swarm-tr.com/tr/c/struct-timespeci-bicimlendirme/941497899/
//This function is used for convert time to nanosecond in %N
int timespec2str(char *buf, uint len, struct timespec *ts)
{
    int ret;
    struct tm t;

    tzset();

    ret = snprintf(&buf[strlen(buf)], len, "%09ld", ts->tv_nsec);
    if (ret >= len)
        return 3;

    return 0;
}
