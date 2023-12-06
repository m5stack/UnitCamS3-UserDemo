// "use-client";

import {
  Card,
  Divider,
  Button,
  Select,
  SelectItem,
  Modal,
  ModalContent,
  ModalHeader,
  ModalBody,
  ModalFooter,
  Code,
} from "@nextui-org/react";
import { useState, useEffect } from "react";
import { useDisclosure } from "@nextui-org/react";
import { posterIntervalList } from "../../assets/poster-interval-list";
import { useNavigate } from "react-router-dom";

type WifiScanState = "Scan WiFi" | "Scanning";

interface ItemWifiSsid {
  key: string;
  label: string;
}

export default function CardShooterConfig() {
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
          navigate("/shooter_start");
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
                  <p>Start interval shooting with following config:</p>

                  <p>
                    <Code color="success">Post interval</Code>{" "}
                    {userConfig.postInterval + "s"}
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
