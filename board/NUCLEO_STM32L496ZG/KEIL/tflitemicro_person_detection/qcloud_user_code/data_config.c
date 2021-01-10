/*-----------------data config start  -------------------*/ 

#define TOTAL_PROPERTY_COUNT 5

static sDataPoint    sg_DataTemplate[TOTAL_PROPERTY_COUNT];

typedef struct _ProductDataDefine {
    TYPE_DEF_TEMPLATE_BOOL m_person;   //有人无人
    TYPE_DEF_TEMPLATE_BOOL m_dagerous; //异常停留
		TYPE_DEF_TEMPLATE_BOOL m_warning;  //报警提示开关
    TYPE_DEF_TEMPLATE_INT  m_count;    //人流量计数
    TYPE_DEF_TEMPLATE_STRING m_name[64+1];
} ProductDataDefine;

static   ProductDataDefine     sg_ProductData;

static void _init_data_template(void)
{
    sg_ProductData.m_person = 0;
    sg_DataTemplate[0].data_property.data = &sg_ProductData.m_person;
    sg_DataTemplate[0].data_property.key  = "person";
    sg_DataTemplate[0].data_property.type = TYPE_TEMPLATE_BOOL;

    sg_ProductData.m_dagerous = 0;
    sg_DataTemplate[1].data_property.data = &sg_ProductData.m_dagerous;
    sg_DataTemplate[1].data_property.key  = "dangerous";
    sg_DataTemplate[1].data_property.type = TYPE_TEMPLATE_BOOL;

    sg_ProductData.m_warning = 0;
    sg_DataTemplate[2].data_property.data = &sg_ProductData.m_warning;
    sg_DataTemplate[2].data_property.key  = "warning";
    sg_DataTemplate[2].data_property.type = TYPE_TEMPLATE_BOOL;	
	  

    sg_ProductData.m_count = 0;
    sg_DataTemplate[3].data_property.data = &sg_ProductData.m_count;
    sg_DataTemplate[3].data_property.key  = "count";
    sg_DataTemplate[3].data_property.type = TYPE_TEMPLATE_INT;
	
    sg_ProductData.m_name[0] = '\0';
    sg_DataTemplate[4].data_property.data = sg_ProductData.m_name;
    sg_DataTemplate[4].data_property.data_buff_len = sizeof(sg_ProductData.m_name)/sizeof(sg_ProductData.m_name[3]);
    sg_DataTemplate[4].data_property.key  = "name";
    sg_DataTemplate[4].data_property.type = TYPE_TEMPLATE_STRING;

};
