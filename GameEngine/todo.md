1. �����ؾ��Ұ�

[] CSM
[] PCSS
[] Fog Light
[] VPL
[] Frustum Culling
[] Materials xml�� ���� �����Ͽ� ����ϵ��� ����
[] xml�� ������ �����Ͽ� �ҷ��� �� �ֵ��� �ý��� ����
[] Killer Feature PCSS �׸���
[] Light Baking (Irradiance? ��� ã�ƺ���)
[] Volumatric Light �߰� (Screen-Space God rays�� ����)
[] Volumatric Cloud
[] Animation
[] �ڿ� �ִϸ��̼�
[] GI

2. ����Ұ�

����Ʈ ���μ��� �¿�� �͵��� 4���� vertex�� ó���ϰ� �ִµ� compute shader���� ó���ص� �������� �ʹ�. �ð� ������ �Ǹ� �ٲ㺸��.

Deferred shading���� �����ص� �������� �ʹ�. (G Buffer�� �̹� �����Ǿ����ִ�.)
Deferred shading���� ����� alpha���� �����ϰ� ��Ƽ���ø��� ���� �˰������� �����ؾ��Ѵ�. �ð��� ���� ����̴�.

------ ���� �����ϴ� ����Ʈ ------

Cascade Shadow Map ������
Cascade Shadow ���� �� Camera UBO
Light UBO
Light - Shadow ��ü���� �����丵

TextureLayer�� ����Ͽ� Shadowmap atlas �����.