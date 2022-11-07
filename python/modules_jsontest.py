import json

data = ""
with open("modules.json", "r") as file:
    data = file.read()

jsondata = json.loads(data)
print(jsondata)
