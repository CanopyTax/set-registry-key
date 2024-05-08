{
  "targets": [
    {
      "target_name": "native",
      "conditions": [
        ["'os'=='win'", {
          "sources": [ "src/set-registry-key.cc" ],
        }]
      ]
    }
  ]
}
