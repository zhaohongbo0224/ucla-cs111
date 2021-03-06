// UCLA CS 111 Lab 1 command internals

enum command_type
  {
    AND_COMMAND,         // A && B
    SEQUENCE_COMMAND,    // A ; B
    OR_COMMAND,          // A || B
    PIPE_COMMAND,        // A | B
    SIMPLE_COMMAND,      // a simple command
    SUBSHELL_COMMAND,    // ( A )
  };

// Data associated with a command.
struct command
{
  enum command_type type;

  // Exit status, or -1 if not known (e.g., because it has not exited yet).
  int status;

  // Since -1 can be shared by ran and unran processes, we explicitly track if this process is ran
  bool finished_running;
  bool running;

  // I/O redirections, or null if none.
  char *input;
  char *output;

  int pid;
  int fd_read_from;   // Specified fd for the command to read from. -1 defaults to STDIN
  int fd_writing_to;  // Signifies the read end of the pipe being written to

  int max_pipe_procs; // Signifies the max number of possible needed concurrent processes due to piping

  // For parallelization, we need to track what exactly our command is dependent on.
  command_t *dependencies;
  size_t dep_alloc_size;  // The size of the allocated array for dependencies
  size_t dep_size;        // How many dependencies we've filled up

  union
  {
    // for AND_COMMAND, SEQUENCE_COMMAND, OR_COMMAND, PIPE_COMMAND:
    struct command *command[2];

    // for SIMPLE_COMMAND:
    char **word;

    // for SUBSHELL_COMMAND:
    struct command *subshell_command;
  } u;

  // The line number the command starts on. Useful for parse errors.
  int line_number;
};

struct command_stream
{
  // An array of command pointers that are parsed from the stream.
  command_t *commands;

  // For iteration (obviously), holds the numerical index of the last traversed location.
  int iterator;

  // The number of commands we've placed in our stream
  int stream_size;

  // The number of spaces we've made available to add commands (through malloc). This should match
  // stream_size when we're all said and done.
  int alloc_size;
};
