# Simulating github action

Install act, see docs for your platform

event.json
```
{
  "ref": "refs/heads/main"
}
```

Run these commands

```
docker pull ghcr.io/catthehacker/ubuntu:full-22.04
act -j pio-build -P ubuntu-latest=ghcr.io/catthehacker/ubuntu:full-22.04 --container-architecture linux/arm64 --reuse -e ./event.json -v


act -j pio-build --container-architecture linux/amd64 -v --reuse -e ./event.json 
```

