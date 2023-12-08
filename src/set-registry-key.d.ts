declare module "@canopytax/set-registry-key" {
  export function setRegistryKey(
    subkey: string,
    valueName: string,
    data: string
  ): boolean;
}
