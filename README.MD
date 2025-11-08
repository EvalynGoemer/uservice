# Universal Service (uservice)

uservice is a small static program that you give systemd style commands and it will translate them to your installed init system

This is useful if you use many distros and swapping between syntax of init systems is annoying

## Supported Systems
- systemd
    - Arch Linux
    - CachyOS
    - Raspberry Pi OS
- openRC
    - Alpine Linux

## Supported Operations
- `uservice start <service>`
- `uservice stop <service>`
- `uservice restart <service>`
- `uservice status <service>`
- `uservice enable <service>`
- `uservice disable <service>`

## Compiling and Installing
Download the repo and run `make` to compile  
Copy the resulting binary to `/bin`
You may or may not need to restart your shell

If you are updating just redownload and repeat the steps and overwrite the file

## Notes
- On OpenRC enabling and disabling services always happens on runlevel `default`
