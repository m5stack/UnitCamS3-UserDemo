interface TimeZoneObject {
  label: string;
  value: string;
}

export const timeZoneList: TimeZoneObject[] = [
  // GMT-...
  { label: "(GMT-12) Pacific/Wake", value: "GMT-12" },
  { label: "(GMT-11) Pacific/Midway", value: "GMT-11" },
  { label: "(GMT-10) Pacific/Honolulu", value: "GMT-10" },
  { label: "(GMT-9) America/Adak", value: "GMT-9" },
  { label: "(GMT-8) America/Anchorage", value: "GMT-8" },
  { label: "(GMT-7) America/Los_Angeles", value: "GMT-7" },
  { label: "(GMT-6) America/Denver", value: "GMT-6" },
  { label: "(GMT-5) America/Chicago", value: "GMT-5" },
  { label: "(GMT-4) America/New_York", value: "GMT-4" },
  { label: "(GMT-3) America/Buenos_Aires", value: "GMT-3" },
  { label: "(GMT-2) America/Noronha", value: "GMT-2" },
  { label: "(GMT-1) Atlantic/Azores", value: "GMT-1" },

  // GMT+...
  { label: "(GMT+0) Europe/London", value: "GMT+0" },
  { label: "(GMT+1) Europe/Berlin", value: "GMT+1" },
  { label: "(GMT+2) Europe/Athens", value: "GMT+2" },
  { label: "(GMT+3) Europe/Moscow", value: "GMT+3" },
  { label: "(GMT+4) Asia/Dubai", value: "GMT+4" },
  { label: "(GMT+5) Asia/Karachi", value: "GMT+5" },
  { label: "(GMT+5:30) Asia/Kolkata", value: "GMT+5:30" },
  { label: "(GMT+6) Asia/Dhaka", value: "GMT+6" },
  { label: "(GMT+7) Asia/Bangkok", value: "GMT+7" },
  { label: "(GMT+8) Asia/Shanghai", value: "GMT+8" },
  { label: "(GMT+9) Asia/Tokyo", value: "GMT+9" },
  { label: "(GMT+9:30) Australia/Adelaide", value: "GMT+9:30" },
  { label: "(GMT+10) Australia/Sydney", value: "GMT+10" },
  { label: "(GMT+11) Pacific/Noumea", value: "GMT+11" },
  { label: "(GMT+12) Pacific/Auckland", value: "GMT+12" },
];
