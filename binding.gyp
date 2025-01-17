{
  "targets": [
    {
      "target_name": "native",
      "type": "<(library)",
      "conditions": [
        ["'os'=='win'", {
          "sources": [ "src/set-registry-key.cc" ],
        }]
      ]
    }
  ]
}
