using cs2sdk;
using Plugify;
using static cs2sdk.cs2sdk;

namespace WithCommands
{
    public class WithCommandsPlugin : Plugin
    {
        private void OnStart()
        {
            // All commands that are prefixed with "cs2_" will automatically be registered as a chat command without the prefix.
            // i.e. `cs2_ping` can be called with `!ping` or `/ping`.
            // Commands can be registered using the instance `AddCommand` method.
            AddCommand("cs2_ping", "Responds to the caller with \"pong\"", 0, OnPingCommand);
        }

        private static int OnPingCommand(int caller, int context, string[] arguments)
        {
            // The player is 0, then the command has been called by the server console.
            if (caller == 0)
            {
                PrintServer("pong server");
                return ResultType.Handled;
            }

            PrintConsole(caller, "pong");
            return ResultType.Handled;
        }
    }
}