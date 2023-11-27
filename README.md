# Simple terminal application for managing your notes at Linux
## Manage your notes in a simple way

BEFORE INSTALLATION MAKE SURE THAT ~/.local/bin EXIST ON YOUR MACHINE. if it not exist create it using command below and make sure its added to your &PATH
mkdir -p ~/.local/bin 

make sure that ~/.local/bin in your &PATH . you can check it threw 
echo $PATH

if ~/.local.bin not in your &PATH add it in there using your SHELL config. you can open your SHELL config with any test editor via terminal (usually its located at ~/.bashrc or ~/.bash_profile) and add it using (in this example BASH) command:
export PATH="/home/<user>/.local/bin:$PATH"

for installation open terminal in directory with notes.out and type

rm -f /usr/.local/bin/notes.out && mv notes.out ~/.local/bin && cd ~/.local/bin && ln -sf notes.out notes && cd -

for running type "notes" in your terminal

