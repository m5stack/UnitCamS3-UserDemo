// "use-client";

import {
  Card,
  Divider,
  Button,
  Input,
  Select,
  SelectItem,
  Modal,
  ModalContent,
  ModalHeader,
  ModalBody,
  ModalFooter,
  Code,
  Dropdown,
  DropdownTrigger,
  DropdownMenu,
  DropdownItem,
} from "@nextui-org/react";
import { useState, useEffect } from "react";
import { useDisclosure } from "@nextui-org/react";
import { posterIntervalList } from "../assets/poster-interval-list";
import { timeZoneList } from "../assets/time-zone-list";
import { useNavigate } from "react-router-dom";

type WifiScanState = "Scan WiFi" | "Scanning";

interface ItemWifiSsid {
  key: string;
  label: string;
}

export default function CardPosterConfig() {
  const navigate = useNavigate();

  const [userConfig, setUserConfig] = useState({
    wifiSsid: "",
    wifiPass: "",
    nickname: "",
    postInterval: "5",
    timeZone: "GMT+0",
  });

  const [widgetStates, setWidgetStates] = useState({
    wifiScanButtonLabel: "Scan WiFi" as WifiScanState,
    saveConfigResultLabel: "",
    wifiSsidList: [] as ItemWifiSsid[],
  });

  const { isOpen, onOpen, onOpenChange } = useDisclosure();

  function fetchDeviceConfig() {
    console.log("fecth device config");
    fetch("/api/v1/get_config")
      .then((response) => response.json())
      .then((data) => {
        console.log(data);
        // Set config to widgets
        setUserConfig({
          ...userConfig,
          wifiSsid: data.wifiSsid,
          wifiPass: data.wifiPass,
          nickname: data.nickname,
          postInterval: data.postInterval,
          timeZone: data.timeZone,
        });
      })
      .catch((error) => {
        if (error instanceof TypeError && error.message.includes("API key")) {
          console.error("Invalid API key:", error);
        } else {
          console.error("There was a problem with the Fetch operation:", error);
        }
      });
  }

  // Fetch config at first time
  useEffect(() => {
    fetchDeviceConfig();
  }, []);

  function handleScanWifi() {
    console.log("start scan wifi");
    setWidgetStates({ ...widgetStates, wifiScanButtonLabel: "Scanning" });

    fetch("/api/v1/get_wifi_list")
      .then((response) => response.json())
      .then((data) => {
        // Remove duplicates
        let uniqueWifiList = [...new Set(data.wifiList as string[])];
        // console.log(uniqueWifiList);

        // Copy and update list
        let newWifiList: ItemWifiSsid[] = [];
        for (var i = 0; i < uniqueWifiList.length; i++) {
          let itemWifissid: ItemWifiSsid = {
            key: uniqueWifiList[i],
            label: uniqueWifiList[i],
          };
          newWifiList.push(itemWifissid);
        }

        setWidgetStates({
          ...widgetStates,
          wifiSsidList: newWifiList,
          wifiScanButtonLabel: "Scan WiFi",
        });
      })
      .catch((error) => {
        if (error instanceof TypeError && error.message.includes("API key")) {
          console.error("Invalid API key:", error);
        } else {
          console.error("There was a problem with the Fetch operation:", error);
        }
        setWidgetStates({ ...widgetStates, wifiScanButtonLabel: "Scan WiFi" });
      });
  }

  function handleSaveConfig() {
    console.log("start save config");

    // Json
    var configJson = JSON.stringify({
      wifiSsid: userConfig.wifiSsid,
      wifiPass: userConfig.wifiPass,
      startPoster: "no",
      nickname: userConfig.nickname,
      postInterval: parseInt(userConfig.postInterval),
      timeZone: userConfig.timeZone,
    });
    console.log("config json:");
    console.log(configJson);

    // Post
    var myHeaders = new Headers();
    myHeaders.append("Content-Type", "application/json");

    fetch("/api/v1/set_config", {
      method: "POST",
      headers: myHeaders,
      body: configJson,
      redirect: "follow",
    })
      .then((response) => response.json())
      .then((data) => {
        console.log(data.msg);
        if (data.msg === "ok") {
          // Jump to page poster start
          navigate("/poster_start");
        } else {
          setWidgetStates({
            ...widgetStates,
            saveConfigResultLabel: "Bad config!",
          });
        }
      })
      .catch((error) => {
        if (error instanceof TypeError && error.message.includes("API key")) {
          console.error("Invalid API key:", error);
        } else {
          console.error("There was a problem with the Fetch operation:", error);
          setWidgetStates({
            ...widgetStates,
            saveConfigResultLabel: "Save config failed!",
          });
        }
      });
  }

  function handleResetConfig() {
    console.log("reset config");

    // Reset config
    fetch("/api/v1/reset_config")
      .then((response) => response.json())
      .then((data) => {
        console.log(data);
        fetchDeviceConfig();
      })
      .catch((error) => {
        if (error instanceof TypeError && error.message.includes("API key")) {
          console.error("Invalid API key:", error);
        } else {
          console.error("There was a problem with the Fetch operation:", error);
        }
      });
  }

  return (
    <Card className="grow">
      {/* wifi configs */}
      <div className="mx-5 mt-5 mb-5 flex flex-col gap-x-5 gap-y-5">
        <div className="flex gap-x-5 justify-end items-center">
          <p className="grow mr-5 text-3xl font-serif font-bold">WiFi</p>

          {/* wifi scan button */}
          <Dropdown
            onOpenChange={(isOpen) => {
              if (isOpen) {
                handleScanWifi();
              }
            }}
            shouldBlockScroll={false}
          >
            <DropdownTrigger>
              <Button
                radius="full"
                variant="light"
                color={
                  widgetStates.wifiScanButtonLabel === "Scanning"
                    ? "warning"
                    : "secondary"
                }
                isLoading={widgetStates.wifiScanButtonLabel === "Scanning"}
              >
                {widgetStates.wifiScanButtonLabel}
              </Button>
            </DropdownTrigger>
            <DropdownMenu
              aria-label="Dynamic Actions"
              items={widgetStates.wifiSsidList}
              onAction={(key) => {
                setUserConfig({ ...userConfig, wifiSsid: key as string });
              }}
            >
              {(item) => (
                <DropdownItem key={item.key}>{item.label}</DropdownItem>
              )}
            </DropdownMenu>
          </Dropdown>
        </div>

        {/* wifi ssid */}
        <Input
          type="wifi-ssid"
          label="SSID"
          className="max-w-xs"
          color="secondary"
          value={userConfig.wifiSsid}
          onValueChange={(value) => {
            setUserConfig({ ...userConfig, wifiSsid: value });
          }}
        ></Input>

        {/* wifi pass */}
        <Input
          type="wifi-pass"
          label="Password"
          className="max-w-xs"
          color="primary"
          value={userConfig.wifiPass}
          onValueChange={(value) => {
            setUserConfig({ ...userConfig, wifiPass: value });
          }}
        ></Input>
      </div>

      <Divider></Divider>

      {/* nickname */}
      <div className="mx-5 mt-5 mb-5 flex flex-col gap-x-5 gap-y-5">
        <p className="grow mr-5 text-3xl font-serif font-bold">Nickname</p>
        <Input
          type="nickname"
          label="Nickname"
          className="max-w-xs"
          color="warning"
          value={userConfig.nickname}
          onValueChange={(value) => {
            if (value.length < 32) {
              setUserConfig({ ...userConfig, nickname: value });
            } else {
              console.log("exceeds limit");
            }
          }}
        ></Input>
      </div>

      <Divider></Divider>

      {/* poster configs */}
      <div className="mx-5 mt-5 mb-5 flex flex-col gap-x-5 gap-y-5">
        <p className="grow mr-5 text-3xl font-serif font-bold">Interval</p>
        <Select
          label="Post Interval"
          className="max-w-xs"
          color="success"
          // defaultSelectedKeys={["30"]}
          defaultSelectedKeys={["5"]}
          onChange={(e) => {
            setUserConfig({ ...userConfig, postInterval: e.target.value });
          }}
        >
          {posterIntervalList.map((posterInterval) => (
            <SelectItem key={posterInterval.value} value={posterInterval.value}>
              {posterInterval.label}
            </SelectItem>
          ))}
        </Select>
      </div>

      <Divider></Divider>

      {/* timezone configs */}
      <div className="mx-5 mt-5 mb-5 flex flex-col gap-x-5 gap-y-5">
        <p className="grow mr-5 text-3xl font-serif font-bold">Time Zone</p>
        <Select
          label="Time zone"
          className="max-w-xs"
          color="primary"
          defaultSelectedKeys={["GMT+0"]}
          onChange={(e) => {
            setUserConfig({ ...userConfig, timeZone: e.target.value });
          }}
        >
          {timeZoneList.map((timeZone) => (
            <SelectItem key={timeZone.value} value={timeZone.value}>
              {timeZone.label}
            </SelectItem>
          ))}
        </Select>
      </div>

      <Divider></Divider>

      {/* start button */}
      <div className="mx-5 mt-5 mb-5 flex flex-col gap-x-5 gap-y-5">
        <div className="flex gap-x-5 items-center">
          <Button
            radius="full"
            variant="flat"
            color="warning"
            size="lg"
            onPress={onOpen}
            className="grow"
          >
            Start
          </Button>
          <Button radius="full" variant="flat" onPress={handleResetConfig}>
            Reset
          </Button>
        </div>

        {/* confirm window */}
        <Modal isOpen={isOpen} onOpenChange={onOpenChange}>
          <ModalContent>
            {(onClose) => (
              <>
                <ModalHeader className="text-3xl font-serif font-bold">
                  Confirm
                </ModalHeader>
                <ModalBody>
                  <p>Start image poster with following config:</p>
                  <p>
                    <Code color="secondary">WiFi SSID</Code>{" "}
                    {userConfig.wifiSsid}
                  </p>
                  <p>
                    <Code color="primary">WiFi passwrod</Code>{" "}
                    {userConfig.wifiPass}
                  </p>
                  <p>
                    <Code color="warning">Nickname</Code> {userConfig.nickname}
                  </p>
                  <p>
                    <Code color="success">Post interval</Code>{" "}
                    {userConfig.postInterval + "s"}
                  </p>
                  <p>
                    <Code color="primary">Time zone</Code>{" "}
                    {userConfig.timeZone}
                  </p>

                  {widgetStates.saveConfigResultLabel != "" && (
                    <p>
                      <Code color="danger">
                        {widgetStates.saveConfigResultLabel}
                      </Code>
                    </p>
                  )}
                </ModalBody>
                <ModalFooter>
                  <Button
                    color="danger"
                    radius="full"
                    variant="light"
                    onPress={onClose}
                  >
                    Close
                  </Button>
                  <Button
                    color="warning"
                    radius="full"
                    variant="flat"
                    // onPress={onClose}
                    onPress={handleSaveConfig}
                  >
                    Yes
                  </Button>
                </ModalFooter>
              </>
            )}
          </ModalContent>
        </Modal>
      </div>
    </Card>
  );
}
