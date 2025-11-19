import subprocess
import os

def get_prompt(shell_path, env=None):
    """
    Launches the shell and captures its prompt.
    """
    try:
        # Launch the shell
        process = subprocess.Popen(
            shell_path,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            env=env
        )

        # Send a newline to trigger the prompt
        process.stdin.write("\n")
        process.stdin.flush()

        # Read the first line of output (the prompt)
        prompt = process.stdout.readline().strip()

        # Terminate the shell
        process.stdin.write("exit\n")
        process.stdin.flush()
        process.wait()

        return prompt
    except Exception as e:
        print(f"Error while testing {shell_path}: {e}")
        return None

def main():
    # Paths to the shells
    bash_path = "/bin/bash"
    # sh42_path = "./42sh"

    # Test with default environment
    print("Testing with default environment:")
    bash_prompt = get_prompt(bash_path)
    # sh42_prompt = get_prompt(sh42_path)
    print(f"Bash Prompt: {bash_prompt}")
   # print(f"42sh Prompt: {sh42_prompt}")

    # Test with empty environment
    print("\nTesting with empty environment:")
    empty_env = {}
    bash_prompt_empty = get_prompt(bash_path, env=empty_env)
   # sh42_prompt_empty = get_prompt(sh42_path, env=empty_env)
    print(f"Bash Prompt (empty env): {bash_prompt_empty}")
    #print(f"42sh Prompt (empty env): {sh42_prompt_empty}")
"""
    # Compare the prompts
    print("\nComparison Results:")
    if bash_prompt == sh42_prompt:
        print("Prompts match with default environment.")
    else:
        print("Prompts differ with default environment.")

    if bash_prompt_empty == sh42_prompt_empty:
        print("Prompts match with empty environment.")
    else:
        print("Prompts differ with empty environment.")
"""
if __name__ == "__main__":
    main()