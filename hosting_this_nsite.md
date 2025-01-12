## Hosting this nsite

### Creating your own credentials

To get started with TollGate, you'll need to set up your nsite configuration. Use the `nsite-cli` command to begin the setup process:

```bash
$ nsite-cli
```

The setup wizard will guide you through the following steps:

1. Enter your NOSTR private key (or press Enter to create a new one)
2. Provide your website or project name
3. Add a project description
4. Configure your Blossom server URLs (default: https://nostr.download)

#### Example Setup Output
```bash
nsite:config Project file not found at .nsite/project.json +0ms
nsite-cli: No existing project configuration found. Setting up a new one:
✔ 1. Existing NOSTR private key (nsec/hex) (Enter to create a NEW one):
✔ 2. Web site or project name: ...
✔ 3. Web site or project description: ...
? 5. Blossom server URLs: https://nostr.download
? 5. Blossom server URLs: Done.
nsite:config Project configuration saved to .nsite/project.json +41s
```

#### Uploading the nsite

* Upload to nsite using the following command:

```bash
~/TollDocs$ nsite-cli upload binaries/
  nsite upload called {
  force: false,
  purge: false,
  publishServerList: false,
  publishRelayList: false,
  publishProfile: false
} +0ms
  nsite Using relays: wss://relay.damus.io, wss://orangesync.tech, wss://nos.lol, wss://relay.primal.net, wss://relay.nostr.band +3ms
Upload for user:       npub1suw0zfxerywd4zku4gjsjde22zhzye9dl2hsll6s3z2qap75p78s66lkhp
Using relays:          wss://relay.damus.io/, wss://orangesync.tech/, wss://nos.lol/, wss://relay.primal.net/, wss://relay.nostr.band/
Publishing relay list (Kind 10002)...
  nsite:nostr relays to broadcast to: [
  'wss://purplepag.es/',
  'wss://user.kindpag.es/',
  'wss://relay.damus.io/',
  'wss://orangesync.tech/',
  'wss://nos.lol/',
  'wss://relay.primal.net/',
  'wss://relay.nostr.band/'
] +0ms
  nsite:nostr relays posted to 6 relays +2s
Publishing profile (Kind 0)...
  nsite:nostr relays to broadcast to: [
  'wss://purplepag.es/',
  'wss://user.kindpag.es/',
  'wss://relay.damus.io/',
  'wss://orangesync.tech/',
  'wss://nos.lol/',
  'wss://relay.primal.net/',
  'wss://relay.nostr.band/'
] +15ms
  nsite:nostr profile posted to 6 relays +738ms
Using blossom servers: https://nostr.download/, https://cdn.hzrd149.com/, https://media-server.slidestr.net/, https://files.v0l.io/
```

* Tell npub1ye5ptcxfyyxl5vjvdjar2ua3f0hynkjzpx552mu5snj3qmx5pzjscpknpr that nsite-cli only works for whitelisted npubs if you get the following error:

```bash
Failed to read error response body: TypeError: body used already for: https://media-server.slidestr.net/upload
    at consumeBody (file:///home/pachai/make_nsite/nsite-cli/node_modules/node-fetch/src/body.js:196:9)
    at Response.text (file:///home/pachai/make_nsite/nsite-cli/node_modules/node-fetch/src/body.js:158:24)
    at processUploads (file:///home/pachai/make_nsite/nsite-cli/dist/upload.js:43:53)
    at process.processTicksAndRejections (node:internal/process/task_queues:95:5)
    at async Command.<anonymous> (file:///home/pachai/make_nsite/nsite-cli/dist/cli.js:152:13)
Error uploading 'index.html' Unexpected token 'P', "Public key"... is not valid JSON
```

You can use this one for now, but expect it to go rogue: https://github.com/OpenTollGate/TollGateNostrToolKit/blob/dockerize/.nsite/project.json#L2

* You can see your upload on URLs of the form: https://npub1suw0zfxerywd4zku4gjsjde22zhzye9dl2hsll6s3z2qap75p78s66lkhp.nsite.lol/

* You may find that the nsite refuses to update frequently. This is because nginx has a 1 hour cache on `nsite.lol`. Your site will update more quickly on `nsite.orangesync.tech`, because traffic is being routed through port 3000 on that machine.

#### Available Commands

Once configured, you can use the following commands:

- `upload <folder>`: Upload files from a directory
- `ls [npub]`: List all files available online
- `download <targetfolder> <npub>`: Download all files available online

For help with any command, use the `-h` or `--help` option.